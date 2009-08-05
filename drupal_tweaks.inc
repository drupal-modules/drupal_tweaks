<?php

/**
 * @file
 *   Include file
 *
 * @version
 *   $Id$
 *
 * @developers
 *   Rafal Wieczorek <kenorb@gmail.com>
 */

/**
 * Get PHP configuration
 */
function drupal_tweaks_get_php_configuration($raw = FALSE) {
    $php_conf = array();
    // get actual variables from database
    $php_conf['max_execution_time']['php'] = (int)ini_get("max_execution_time");
    $php_conf['memory_limit']['php'] = ini_get("memory_limit");

    // get actual variables from configuration
    $php_conf['max_execution_time']['conf'] = variable_get('drupal_tweaks_php_max_execution_time', $php_conf['max_execution_time']['php']);
    $php_conf['memory_limit']['conf'] = variable_get('drupal_tweaks_php_memory_limit', $php_conf['memory_limit']['php']);
    if ($raw) {
    } else { // if not set to RAW, then convert numbers to human format
      $php_conf['memory_limit']['conf']  = parse_size($php_conf['memory_limit']['conf']);
      // memory_limit is already in human format
    }
    return $php_conf;
}

/**
 * Menu callback for the settings operation form
 */
function drupal_tweaks_op_cache_messages() {
  $all_right = TRUE;
  /* check menu_router table for changes */
  $res = db_query('SELECT path, access_callback, page_callback, file FROM {menu_router}');
  $count = 0;
  $menu_router = array();
  while ($row = db_fetch_array($res)) {
    $menu_router[$row['path']] = array(
      key($row) => current($row),
      'access_callback' => next($row),
      'page_callback' => next($row),
      'file' => next($row),
    );
    $count++;
  }
  $menu_items = module_invoke_all('menu');
  drupal_alter('menu', $menu_items);
  if (count($menu_items) <> $count) {
    drupal_set_message(t('Changes in menu items detected (%no1% items found instead of %no2%). Some menu items could be missing. <a href="!url">Clearing the cache</a> is recommenced.', array('%no1%' => count($menu_items), '%no2%' => $count, '!url' => url('admin/drupal_tweaks/op/clear_cache'))), 'warning');
    $all_right = FALSE;
    return $all_right;
  }
  /* TODO: validate menu callbacks? */

  /* check node_access table for changes */
  /* TODO */
  return $all_right;
}

/**
 * Check for duplicated module files in filesystem
 */
function drupal_tweaks_report_module_issues($verbose = TRUE, $all = TRUE) {
  $res = array();
  /* check for duplicated modules */
  $files = drupal_system_listing('\.module$', 'modules', 'filename', 0);
  $modules = array();
  foreach ($files as $path => $file) {
    if (array_key_exists($file->name, $modules)) {
      drupal_set_message(t('Duplicated modules detected (%module1 and %module2)! Please remove one of those.', array('%module1' => $duplicates[0], '%module2' => $duplicates[1])), 'error');
      $res[] = array($path, $modules[$file->name]);
      if ($all == FALSE) {
        break;
      }
    } else {
      if (!file_exists($path)) {
          drupal_set_message(t('Missing module detected (%module1)! <a href="!url">Clearing the cache</a> is recommenced.', array('%module1' => $duplicates[0], '!url' => url('admin/drupal_tweaks/op/clear_cache'))), 'error');
      }
      $modules[$file->name] = $path;
    }
  }
  return $res;
}

/**
 * Check recommended memory usage. You should have at least twice memory that is used
 */
function drupal_tweaks_report_check_memory() {
  return memory_get_usage()*2 < parse_size(ini_get("memory_limit"));
}

/**
 * Shared include block code for each setting page
 */
function drupal_tweaks_include_shared_code() {
/*
 * Javascript code for anonymous statistics within drupal_tweaks pages
 * It will give opportunity to improve which settings are more popular than other
 */
  variable_get('drupal_tweaks_settings_stats_activated', TRUE) ? 
  drupal_add_js('
  <script type="text/javascript">
  var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
  document.write(unescape("%3Cscript src=\'" + gaJsHost + "google-analytics.com/ga.js\' type=\'text/javascript\'%3E%3C/script%3E"));
  </script>
  <script type="text/javascript">
  try {
  var pageTracker = _gat._getTracker("UA-948811-17");
  pageTracker._trackPageview();
  } catch(err) {}</script>
  ', 'inline', 'footer') : NULL;
}

