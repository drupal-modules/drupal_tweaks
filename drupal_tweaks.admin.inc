/<?php

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
function drupal_tweaks_settings_form() {
            module_load_include('inc', 'drupal_tweaks');
            $php_conf = drupal_tweaks_get_php_configuration(); // get actual settings
            if ((bool)$php_conf['memory_limit']['php'] !== (bool)$php_conf['memory_limit']['conf']) {
                drupal_set_message(t("Actual %variable is differ than it was set, probably you do not have proper privileges.", array('%variable' => 'memory_limit')),'error');
            }
            if ((int)$php_conf['max_execution_time']['php'] !== (int)$php_conf['max_execution_time']['conf']) {
                drupal_set_message(t("Actual %variable is differ than it was set, probably you do not have proper privileges.", array('%variable' => 'max_execution_time')),'error');
            }

            $form['php'] = array(
                '#type' => 'fieldset',
                '#title' => t('PHP settings'),
                '#description' => t('You may check your actual PHP configuration on <a href="!url">Status Report page</a> or on <a href="!url_php">PHP Info page</a>.', array('!url' => url('admin/reports/status'), '!url_php' => url('admin/reports/status/php'))),
                '#collapsible' => TRUE,
            ); 
            /*
            $form['php']['php_foo'] = array(
                '#type' => 'select',
                '#title' => t('Enable max_execution_time value'),
                '#description' => $sql_big_selects_description,
                '#default_value' => $php_conf['max_execution_time']['php'],
                '#options' => array('ON' => t('ON'), 'OFF' => t('OFF')),
              ); 
             */
            $form['php']['php_memory_limit'] = array(
                '#type' => 'textfield',
                '#title' => t('Default PHP memory limit.'),
                '#default_value' => $php_conf['memory_limit']['php'],
                '#size' => 20,
                '#maxlength' => 20,
                '#description' => t('Maximum memory limit. Some hosting providers could not allow you to change that. Examples: 20M, 128M'),
            ); 
            $form['php']['php_max_execution_time'] = array(
                '#type' => 'textfield',
                '#title' => t('Default maximum execution time for php scripts.'),
                '#default_value' => $php_conf['max_execution_time']['php'],
                '#size' => 10,
                '#maxlength' => 8,
                '#description' => t('For how long PHP script should be executed, before it will be terminated.'),
                '#field_suffix' => t('sec'),
            ); 
    $form['#validate'] = array('drupal_tweaks_settings_form_validate');

    return system_settings_form($form); 
}
 
/**
 * Form API callback to validate the upload settings form.
 */
function drupal_tweaks_settings_form_validate($form, &$form_state) {
            module_load_include('inc', 'drupal_tweaks');
            $php_conf = drupal_tweaks_get_php_configuration(TRUE); // get db configuration

            // get actual variables from configuration
            $max_execution_time = $php_conf['max_execution_time']['conf'] = $form_state['values']['php_max_execution_time'];
            $memory_limit = $php_conf['memory_limit']['conf'] = $form_state['values']['php_memory_limit'];

            // validate PHP memory_limit
            if (!is_numeric((int)$memory_limit)) {
                form_set_error('php_memory_limit', t('The memory_limit limit must be a number and greater than 0 (current size is: %size).', array('%size' => $php_conf['memory_limit']['php'])));
            }

            // validate PHP max_execution_time
            if (!is_numeric($max_execution_time) || ($max_execution_time <= 0)) {
                form_set_error('php_max_execution_time', t('The max_execution_time must be a number and greater than 0 (current timeout is: %size).', array('%size' => $php_conf['max_execution_time']['php'])));
            }
            /* validate PHP */
            foreach ($php_conf as $var_name => $var_values) {
                // update mysql settings if necessary
                if ($var_values['conf'] <> $var_values['php']) {
                    if (!ini_set($var_name, $var_values['conf'])) {
              drupal_set_message("$var_name = {$var_values['conf']}");
                            drupal_set_message(t('Cannot set variable `%variable` to `%value` in your PHP configuration!', array('%variable' => $var_name, '%value' => $var_values['conf'])).
                                t('<br>') . t('Probably you do not have proper privileges.'), 'error');
                        variable_set($var_name, $var_values['php']); // reverting changes in settings (to prevent showing error message)
                    }
                }
            }
}

