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
function drupal_tweaks_get_php_configuration($raw = FALSE, $name = NULL) {
    static $php_conf_org = array();
    $php_conf = array();
    // get actual variables from database
    $php_conf['max_execution_time']['php'] = (int)ini_get("max_execution_time");
    $php_conf['memory_limit']['php'] = ini_get("memory_limit");
    // $php_conf['upload_max_filesize']['php'] = ini_get("upload_max_filesize"); /* not changeable */

    // get actual variables from configuration
    $php_conf['max_execution_time']['conf'] = variable_get('drupal_tweaks_php_max_execution_time', $php_conf['max_execution_time']['php']);
    $php_conf['memory_limit']['conf'] = variable_get('drupal_tweaks_php_memory_limit', $php_conf['memory_limit']['php']);

    if (!$php_conf_org) {
      $php_conf_org['max_execution_time']['php'] = $php_conf['max_execution_time']['php'];
      $php_conf_org['memory_limit']['php'] = $php_conf['memory_limit']['php'];
    }
    // get original variables from configuration
    $php_conf['max_execution_time']['org'] = $php_conf_org['max_execution_time']['php'];
    $php_conf['memory_limit']['org'] = $php_conf_org['memory_limit']['php'];

    if ($raw) {
    } else { // if not set to RAW, then convert numbers to human format
      $php_conf['memory_limit']['conf']  = parse_size($php_conf['memory_limit']['conf']);
      $php_conf['upload_max_filesize']['conf']  = parse_size($php_conf['upload_max_filesize']['conf']);
      // memory_limit is already in human format
    }
    return $name ? $php_conf[$name] : $php_conf;
}

/**
 * Menu callback for the settings operation form
 */
function drupal_tweaks_op_cache_messages() {
  $all_right = TRUE;
  /* check menu_router table for changes */
  $res = db_query('SELECT path, access_callback, page_callback, file FROM {menu_router}');
  $menu_router = array();
  while ($row = db_fetch_array($res)) {
    $menu_router[$row['path']] = array(
      key($row) => current($row),
      'access_callback' => next($row),
      'page_callback' => next($row),
      'file' => next($row),
    );
  }
  $menu_items = menu_router_build();
  drupal_alter('menu', $menu_items);
//var_dump(array_keys($menu_router)); exit;
//var_dump(array_keys($menu_items)); exit;
  if (count($menu_items) <> count($menu_router)) {
    drupal_set_message(t('Changes in menu items detected (%no1% items found instead of %no2%). Some menu items could be missing. <a href="!url">Clearing the cache</a> is recommended.', array('%no1%' => count($menu_items), '%no2%' => count($menu_router), '!url' => url('admin/drupal_tweaks/op/clear_cache'))), 'warning');
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
      drupal_set_message(t('Duplicated modules detected (%module1 and %module2)! Please remove one of those.', array('%module1' => $path, '%module2' => $modules[$file->name])), 'error');
      $res[] = array($path, $modules[$file->name]);
      if ($all == FALSE) {
        break;
      }
    } else {
      if (!file_exists($path)) {
          drupal_set_message(t('Missing module detected (%module1)! <a href="!url">Clearing the cache</a> is recommended.', array('%module1' => $path, '!url' => url('admin/drupal_tweaks/op/clear_cache'))), 'error');
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
  var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
  document.write(unescape("%3Cscript src=\'" + gaJsHost + "google-analytics.com/ga.js\' type=\'text/javascript\'%3E%3C/script%3E"));
  var pageTracker;
  setTimeout("startGA();", 500);
  function startGA()
  {
    try {
    var pageTracker = _gat._getTracker("UA-948811-17");
    pageTracker._setDomainName("none");
    pageTracker._setAllowLinker(true);
    pageTracker._trackPageview();
    } catch(err) {}
  }
  ', 'inline') : NULL;
  drupal_add_js('pageTracker._trackPageview(location.pathname);', 'inline');
}

/**
 * Check the message
 */
function drupal_tweaks_msgs_give_advice($msg, $type) {
  if (variable_get('drupal_tweaks_give_advice', TRUE))
  switch ($type) {
    case 'error':
      if (strpos($msg, 'user warning: Table') !== FALSE) { // 'user warning: Table 'xxx' doesn't exist query'
        preg_match("/FROM (?P<table_name>[a-z0-9_]+) /", $msg, $matches);
        $table_name = $matches['table_name'];
        //$module_name = TODO;
        //drupal_set_message(t('It\'s recommended to reinstall this module to fix that problem.'), 'warning');
        /* FIXME: Somebody interested to apply a proper code? */
      } 

      /* Note: Some of the PHP errors are not catchable; Example: warning: Wrong parameter count for strpos() in... */
    break;
    case 'warning':
    break;
  }
}

/**
 * Check for changes provided by module and clear the caches
 */
function drupal_tweaks_module_check_db_changes($module_name) {
  if ($menus = module_invoke(array($module_name), 'menu')) {
    drupal_set_message(t('%module: New %type items detected!', array('%module' => $module_name, '%type' => 'menu')));
    cache_clear_all(NULL, 'cache_menu');
  } else {
    drupal_tweaks_op_cache_messages();
  }
  if ($blocks = module_invoke($module_name, 'block')) {
    drupal_set_message(t('%module: New %type items detected!', array('%module' => $module_name, '%type' => 'block')));
    cache_clear_all(NULL, 'cache_block');
  }
  if ($filters = module_invoke($module_name, 'filter')) {
    drupal_set_message(t('%module: New %type items detected!', array('%module' => $module_name, '%type' => 'filter')));
    cache_clear_all(NULL, 'cache_filter');
  }
  if ($perms = module_invoke($module_name, 'perm')) {
    drupal_set_message(t('%module: New %type items detected! (%data)', array('%module' => $module_name, '%type' => 'permission', '%data' => print_r($perms, true))));
  }
}

/**
 * Return current backtrace
 */
function drupal_tweaks_get_backtrace($limit = 100, $nl = "<br>\n", $tab = ".") {
  $backtrace = debug_backtrace(TRUE);
  if (is_array($backtrace)) {
    array_shift($backtrace); // remove this function from array
    array_shift($backtrace); // remove drupal_tweaks_show_backtrace/drupal_tweaks_log_backtrace from array
    array_shift($backtrace); // remove drupal_tweaks_watchdog
    array_shift($backtrace); // remove call_user_func_array(module.inc:450)
    array_shift($backtrace); // remove module_invoke(bootstrap.inc:839)
    array_shift($backtrace); // remove watchdog(common.inc:619)
    array_shift($backtrace); // drupal_error_handler(genmod.install:326) 
    $buffer = '';
    $counter = 0;
    foreach ($backtrace as $index => $function) {
        $function_name = $function['function'];
        $line = !empty($backtrace[$index]['line']) ? $backtrace[$index]['line'] : '?';
        $filename = !empty($backtrace[$index]['file']) ? basename($backtrace[$index]['file']) : '?';
        $args = NULL;
        if (isset($function['args'])) {
          $args = is_string($function['args']) ? $function['args'] : serialize($function['args']); // serialize arrays and objects
        }
        $args = (!empty($limit) && strlen($args) > $limit) ? gettype($args) : $args;
        $buffer .= str_repeat($tab, $counter++);
        $buffer .= "$function_name($args)[$filename:$line];" . $nl;
    }
    $buffer .= str_repeat($tab, $counter++);
    $buffer .= 'index.php';
  }
  if (empty($buffer)) {
    $buffer = 'n/a';
  }
  return $buffer;
}

/**
 * Add backtrace to drupal_set_message
 *
 */
function drupal_tweaks_show_backtrace() {
    $bt_code = "<br>\nBacktrace:<br>\n" . drupal_tweaks_get_backtrace(variable_get('drupal_tweaks_backtrace_log_arg_max_length', 1000));
    $_SESSION['messages']['error'][count($_SESSION['messages']['error'])-1] .= $bt_code;
}

/**
 * Log backtrace into Drupal watchdog
 *
 */
function drupal_tweaks_log_backtrace($log) {
    $bt_code = "<br>\nBacktrace:<br>\n" . drupal_tweaks_get_backtrace(variable_get('drupal_tweaks_backtrace_log_arg_max_length_log', 1000));
    watchdog('backtrace', t('Type: ') . $log['type'] . "\n<br>" . $log['message'] . ' ' . $bt_code, $log['variables']);
}

