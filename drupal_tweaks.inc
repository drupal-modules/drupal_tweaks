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

