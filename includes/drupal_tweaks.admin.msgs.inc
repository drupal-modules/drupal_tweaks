<?php

/**
 * @file
 *   Log specified drupal messages instead of showing them
 *
 *   Practical usage:
 *     - hide PHP/MySQL sytax error messages from anonymous/non-admin users (for security reasons)
 *     - create specified actions when specified message show up
 *       Example: send fatal errors directly to webmaster e-mail address to fix the errors immediately (Triggers & Actions)
 *     - log all messages to check what other users see and what common problems they have (i.e. with filling the forms)
 *     - Ever wonder how to hide: 'The configuration options have been saved.' or default messages after node saving?
 *       Get rid of not needed messages instead of translating them to empty strings.
 *     - also good for development and testing purposes to log messages from specified modules or with specified rules
 *     - and many more ...
 *
 * @version
 *   $Id$
 *
 * @developers:
 *    Rafal Wieczorek <kenorb@gmail.com>
 */

define(REGEX_HELP, "http://php.net/manual/en/book.pcre.php");

/**
 * Menu callback for the settings form.
 */
function drupal_tweaks_msgs_settings_form() {
  module_load_include('inc', 'drupal_tweaks'); // load additional functions from included file
  drupal_tweaks_include_shared_code();

  $form['drupal_tweaks_msgs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages tweaks'),
    '#description' => t('You can configure here different behaviours for messages.'),
  ); 

  $form['drupal_tweaks_msgs']['msg2log'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log Rules'),
    '#description' => t("Choose which messages should be logged instead of showing them.<br>If you want to configure those settings for specified roles, configure it on <a href='!url'>Permission Page</a>.", array('!url' => url('admin/user/permissions'))),
    '#collapsible' => TRUE,
  ); 

  $form['drupal_tweaks_msgs']['msg2log']['drupal_tweaks_msg2log_activated'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate message rules.'),
    '#description' => t('Select if you want to activate below log rules.'),
    '#default_value' => variable_get('drupal_tweaks_msg2log_activated', FALSE),
  );

  /* ERROR MESSAGES */
  foreach (array('error', 'warning', 'status') as $type) {
    $form['drupal_tweaks_msgs']['msg2log'][$type] = array(
      '#type' => 'fieldset',
      '#title' => t('Log rules for %type messages.', array('%type' => $type)),
      '#collapsible' => TRUE,
    ); 

    $form['drupal_tweaks_msgs']['msg2log'][$type]['drupal_tweaks_msgs_' . $type . '_regex'] = array(
      '#type' => 'textarea',
      '#title' => t('Log followed %type messages instead of showing them to the user', array('%type' => $type)),
      '#cols' => 60,
      '#rows' => 5,
      '#default_value' => variable_get('drupal_tweaks_msgs_' . $type . '_regex', implode("\n", drupal_tweaks_msg2log_settings_default_regex_rules($type))),
      '#description' => t('Enter the RegEx rules which messages should be converted to logs.'),
      '#wysiwyg' => FALSE,
    );

    if (!empty($_SESSION['drupal_tweaks_messages'][$type])) {
      list($passed_test_msgs, $failed_test_msgs, $msgs_toolong) = drupal_tweaks_msgs_get_test_msgs($type, variable_get('drupal_tweaks_msgs_' . $type . '_regex', implode("\n", drupal_tweaks_msg2log_settings_default_regex_rules($type))), $_SESSION['drupal_tweaks_messages']);
      $test_msgs = !empty($passed_test_msgs) ? t('PASSED:') . "\n" . implode("\n", $passed_test_msgs) . "\n\n" : '';
      $test_msgs .= !empty($failed_test_msgs) ? t('FAILED:') . "\n" . implode("\n", $failed_test_msgs) . "\n\n" : '';
      $test_msgs .= !empty($msgs_toolong) ? t('TOO LONG:') . "\n" . implode("\n", $msgs_toolong) : '';
    }
    if (variable_get('drupal_tweaks_msg2log_activated', FALSE)) {
      $form['drupal_tweaks_msgs']['msg2log'][$type]['drupal_tweaks_msgs_' . $type . '_regex_test'] = array(
        '#type' => 'textarea',
        '#cols' => 60,
        '#rows' => min(count($passed_test_msgs)+count($failed_test_msgs)+2, 10),
        '#attributes' => array('readonly' => 'readonly', 'disabled' => 'disabled'),
        '#value' => !empty($test_msgs) ? $test_msgs : t('No messages captured. Please go to the page with messages to capture them.'),
        '#description' => t('Show sample messages.'),
        '#wysiwyg' => FALSE,
      );
    }
  }

  if (!variable_get('drupal_tweaks_msg2log_activated', FALSE)) {
    drupal_set_message(t('Please enable log rules option to apply it on your website.'), 'warning');
  }

  $form['#validate'] = array('drupal_tweaks_msgs_settings_form_validate');

  return system_settings_form($form); 
}

/**
 * Form API callback to validate the settings form.
 */
function drupal_tweaks_msgs_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  /* check message rules */
  foreach (array('error', 'warning', 'status') as $var_name) {
    $var_name = 'drupal_tweaks_msgs_' . $msg_type . '_regex';
    $myerrors = array();
    $rules = preg_split('/\n/', $values[$var_name]);
    foreach ($rules as $line => $rule) {
      $rule = trim($rule);
      if (!empty($rule) && preg_match($rule, '', $matches) === FALSE) {
        $myerrors[] = t("Rule '%rule' on line %line is invalid! Read more: <a href='!url'>preg_match()</a>", array('%rule' => $rule, '%line' => ++$line, '!url' => REGEX_HELP));
      }
    }
    if (!empty($myerrors)) {
      form_set_error($var_name, implode('<br/>', $myerrors)); // show syntax errors
    }
  }
} 

/**
 * Get messages of specified type which passed $rules from $msgs list
 */
function drupal_tweaks_msgs_get_test_msgs($type, $rules = NULL, $msgs = array()) {
  empty($msgs) ? $msgs = &$_SESSION['drupal_tweaks_messages'] : NULL;
  $res_passed = $res_failed = array();
  /* remove duplicates */
  $msgs[$type] = array_unique($msgs[$type]);
  /* limit test messages to 20 */
  if (count($msgs[$type]) > 20) {
    for ($i = 0; count($msgs[$type])-20; $i++) {
      array_shift($msgs[$type]);
    }
  }
  foreach ($msgs[$type] as $key => &$message) {
    $rule_passed = drupal_tweaks_msg2log_check_rule($message, $type) ? TRUE : FALSE;
    $rule_passed ? $res_passed[] = $message : $res_failed[] = $message;
    /* activate it, if you want ignore too long messages
      if (strlen($message) > 1000) {
        $res_toolong[] = $message;
      }
   */
  }
  return array($res_passed, $res_failed, $res_toolong);
}


