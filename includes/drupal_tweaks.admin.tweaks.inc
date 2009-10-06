<?php

/**
 * @file
 *   Settings Page for module tweaks
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
function drupal_tweaks_general_tweaks_form() {
  module_load_include('inc', 'drupal_tweaks'); // load additional functions from included file
  drupal_tweaks_include_shared_code();

  $form['drupal_tweaks_dev'] = array(
    '#type' => 'fieldset',
    '#title' => t('Development tweaks'),
    '#description' => t('Tweaks which help you to diagnose common problems with your website.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['drupal_tweaks_dev']['drupal_tweaks_backtrace_show'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show backtrace on error.'),
    '#description' => t('Show backtrace under Drupal error messages. It will help you to diagnose what\'s the reason of the error.'),
    '#default_value' => variable_get('drupal_tweaks_backtrace_show', FALSE),
  );

  $form['drupal_tweaks_dev']['drupal_tweaks_backtrace_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log backtrace on error.'),
    '#description' => t('Log backtrace on Drupal error messages. (Link: <a href="!url">Recent log entries</a>)', array('!url' => url('admin/reports/dblog'))),
    '#default_value' => variable_get('drupal_tweaks_backtrace_show', TRUE),
  );

  $form['drupal_tweaks_dev']['drupal_tweaks_backtrace_adv'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced development settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['drupal_tweaks_dev']['drupal_tweaks_backtrace_adv']['drupal_tweaks_backtrace_log_arg_max_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum length of function arguments in backtrace.'),
    '#description' => t('If you want to debug website and check which arguments has been passed into function, try to increase it.'),
    '#default_value' => variable_get('drupal_tweaks_backtrace_log_arg_max_length', 20),
    '#size' => 5,
  );

  $form['drupal_tweaks_modules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Module tweaks'),
    '#description' => t('Tweaks for modules.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $module_name = 'Filter';
  $form['drupal_tweaks_modules']['drupal_tweaks_filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('%module tweaks', array('%module' => $module_name)),
    '#description' => t('Tweaks for %module module.', array('%module' => $module_name)),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#disabled' => !module_exists('filter'),
  );
  $form['drupal_tweaks_modules']['drupal_tweaks_filter']['drupal_tweaks_filter_tweak'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable tweaks for %module module.', array('%module' => $module_name)),
    '#description' => t('If selected, you will see which filter belongs to which module (Link: <a target=_blank href="!url">Filter list</a> and choose configure)', array('!url' => url('admin/settings/filters'))),
    '#default_value' => variable_get('drupal_tweaks_filter_tweak', module_exists('filter')),
    '#disabled' => !module_exists('filter'),
  );

  $module_name = 'Views';
  $form['drupal_tweaks_modules']['drupal_tweaks_views'] = array(
    '#type' => 'fieldset',
    '#title' => t('%module tweaks', array('%module' => $module_name)),
    '#description' => t('Tweaks for %module module.', array('%module' => $module_name)),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#disabled' => !module_exists('views'),
  );
  $form['drupal_tweaks_modules']['drupal_tweaks_views']['drupal_tweaks_views_tweak'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable tweaks for %module module.', array('%module' => $module_name)),
    '#description' => t('If selected, you will see which View belongs to which module (Link: <a target=_blank href="!url">Views list</a>)', array('!url' => url('admin/build/views'))),
    '#default_value' => variable_get('drupal_tweaks_views_tweak', module_exists('filter')),
    '#disabled' => !module_exists('views'),
  );

  return system_settings_form($form); 
}

