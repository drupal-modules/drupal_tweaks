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

