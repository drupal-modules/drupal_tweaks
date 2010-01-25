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
 * Menu callback for the settings form.
 */
function inline_msg_admin_settings() {
  if (module_exists('filter')) {
    $module_name = 'Filter';
    $form['inline_msg_filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('%module tweaks', array('%module' => $module_name)),
      '#description' => t('Tweaks for %module module.', array('%module' => $module_name)),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['inline_msg_filter']['inline_msg_filter_tweak'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable tweaks for %module module.', array('%module' => $module_name)),
      '#description' => t('If selected, you will see which filter belongs to which module (Link: <a target=_blank href="!url">Filter list</a> and choose configure)', array('!url' => url('admin/settings/filters'))),
      '#default_value' => variable_get('inline_msg_filter_tweak', TRUE),
    );
  }
  if (module_exists('views')) {
    $module_name = 'Views';
    $form['inline_msg_views'] = array(
      '#type' => 'fieldset',
      '#title' => t('%module tweaks', array('%module' => $module_name)),
      '#description' => t('Tweaks for %module module.', array('%module' => $module_name)),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['inline_msg_views']['inline_msg_views_tweak'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable tweaks for %module module.', array('%module' => $module_name)),
      '#description' => t('If selected, you will see which View belongs to which module (Link: <a target=_blank href="!url">Views list</a>)', array('!url' => url('admin/build/views'))),
      '#default_value' => variable_get('inline_msg_views_tweak', TRUE),
    );
  }

  return system_settings_form($form); 
}

