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
 * Menu callback for the settings general operation form.
 */
function drupal_tweaks_general_op_form() {
  module_load_include('inc', 'drupal_tweaks'); // load additional functions from included file
  drupal_tweaks_include_shared_code();
  // drupal_add_js(drupal_get_path('module', 'drupal_tweaks') . '/js/drupal_tweaks.general.js'); // ?

  /* NODE OPERATIONS */
  $form['drupal_tweaks_nodes_op'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node quick operations'),
    '#description' => t('You can also do it manually on <a target=_blank href="!url">Content Page</a>.', array('!url' => url('admin/content/node'))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['drupal_tweaks_nodes_op']['drupal_tweaks_node'] = array(
    '#type' => 'textfield',
    '#title' => t('Find the node.'),
    '#description' => t('Type node which you want to find.'),
    '#autocomplete_path' => 'admin/drupal_tweaks/autocomplete/nodes',
    '#size' => 30,
    '#maxlength' => 60,
    //'#suffix' => t('Under development'),
  );
  $form['drupal_tweaks_user_op'] = array(
    '#type' => 'fieldset',
    '#title' => t('User quick operations'),
    '#description' => t('You can also do it manually on <a target=_blank href="!url">User List Page</a>.', array('!url' => url('admin/user/user'))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['drupal_tweaks_user_op']['drupal_tweaks_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Find the user.'),
    '#description' => t('Type user which you want to find.'),
    '#autocomplete_path' => 'admin/drupal_tweaks/autocomplete/users',
    '#size' => 30,
    '#maxlength' => 60,
    //'#suffix' => t('Under development'),
  );

  return $form;
}

/**
 * Menu callback for the settings general operation form.
 */
function drupal_tweaks_general_op_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  switch ($values['op']) {
    case t('Save'):
    break;
  }
}

