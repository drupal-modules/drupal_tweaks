<?php

/**
 * @file
 *   Form with Settings
 *
 * @version
 *   $Id$
 *
 * @developers
 *   Rafal Wieczorek <kenorb@gmail.com>
 */


/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions for modules which can be enabled or disabled
 */
function drupal_tweaks_autocomplete_modules($string = '') {
  $status = (int)(arg(3) == 'enabled_modules');
  $matches = array();
  if ($string) {
    $result = db_query_range("SELECT name, info FROM {system} WHERE type = 'module' AND status = %d AND name LIKE LOWER('%s%%') ORDER BY weight ASC, filename ASC", $status, $string, 0, 10);
    while ($row = db_fetch_object($result)) {
      $info = unserialize($row->info);
      $matches[$row->name] = check_plain($info['name'] . ' - ' . $info['description']);
    }
  }

  drupal_json($matches);
}

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions for nodes
 */
function drupal_tweaks_autocomplete_nodes($string = '') {
  $matches = array();
  if ($string) {
    $result = db_query_range("SELECT nid, title FROM {node} WHERE title LIKE LOWER('%s%%') ORDER BY changed ASC", $string, 0, 10);
    while ($row = db_fetch_object($result)) {
      $matches[$row->title] = check_plain($row->nid . ': ' . $row->title);
    }
  }

  drupal_json($matches);
}

