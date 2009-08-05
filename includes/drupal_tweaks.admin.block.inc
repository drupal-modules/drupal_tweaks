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
 * Menu callback for the settings operation form
 */
function drupal_tweaks_block_form() {
  module_load_include('inc', 'drupal_tweaks'); // load additional functions from included file
  drupal_tweaks_include_shared_code();

  $form['drupal_tweaks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Block operations'),
    '#collapsible' => TRUE,
  ); 

  return system_settings_form($form); 
}

