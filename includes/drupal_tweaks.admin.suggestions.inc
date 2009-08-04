<?php

/**
 * @file
 *   Form with Report Settings
 *
 * @version
 *   $Id$
 *
 * @developers
 *   Rafal Wieczorek <kenorb@gmail.com>
 */

/**
 * Menu callback for the error reporting form
 */
function drupal_tweaks_report_form() {
  $no_problems = TRUE;
  module_load_include('inc', 'drupal_tweaks'); // load additional function from included file
  $no_problems = $no_problems && drupal_tweaks_op_cache_messages(); // Defined in drupal_tweaks.inc

  /* FIXME: Checking new updates */
  require_once './includes/install.inc';
  $modules = drupal_get_installed_schema_version(NULL, FALSE, TRUE);
  foreach ($modules as $module => $schema_version) {
    if ($updates = drupal_get_schema_versions($module)) {
      foreach (array_keys($updates) as $update) {
        if ($update > $schema_version) {
          $no_problems = $no_problems && drupal_set_message(t('New updates available for %module module.', array('%module' => $module)));
          break;
        }
      }
    }
  }

  if ($no_problems) {
    drupal_set_message(t('No problem detected.'));
  }
  return $form;
}

