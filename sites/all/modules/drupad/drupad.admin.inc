<?php

/**
 * @file
 * Drupad configuration
 */

function drupad_settings_form($form_state) {
  $form = array();

  // Services
  $form['drupad_services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enabled services'),
    '#description' => t("Select features you want your site to expose to the application. Users must have the permission <strong>use Drupad application</strong> to be able to login form their device. If a feature below is not enabled, it won't be shown on the application regardless of the permission, even for <em>site maintenance account</em>."),
    '#collapsible' => TRUE,
  );

  foreach (drupad_available_callbacks() as $callback => $service) {

    if (!isset($service['disabled'])) {
      $disabled = FALSE;
    }
    else {
      $disabled = $service['disabled'] == TRUE ? TRUE : FALSE;
    }

    if (!$disabled) {
      $default_value = variable_get('drupad_callback_' . $callback, 1);
    }
    else {
      variable_set('drupad_callback_' . $callback, 0);
      $default_value = 0;
    }

    $form['drupad_services']['drupad_callback_' . $callback] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($service['title']),
      '#description' => filter_xss($service['description']),
      '#default_value' => $default_value,
      '#disabled' => $disabled,
    );
  }

  // Comments options
  $form['comments_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Comment options'),
    '#collapsible' => TRUE,
    '#states' => array(
      // Hide the settings when the cancel notify checkbox is disabled.
      'invisible' => array(
        'input[name="drupad_callback_comments"]' => array('checked' => FALSE),
      ),
    ),
  );

  $comments_entities = entity_get_info('comment');

  if ($comments_entities['fieldable']) {
    foreach ($comments_entities['bundles'] as $bundle_name => $bundle_info) {
      // Gather bundle information.
      $instances = field_info_instances('comment', $bundle_name);
      $options = array();
      foreach ($instances as $name => $instance) {
        // We only can handle textarea field types
        if ($instance['widget']['module'] == 'text' && $instance['widget']['type'] == 'text_textarea') {
          $options[$name] = $instance['label'];
        }
      }

      // Build default option
      if (isset($instances['comment_body'])
          && $instances['comment_body']['widget']['module'] == 'text'
          && $instances['comment_body']['widget']['type'] == 'text_textarea'
          && variable_get('drupad_comment_field_' . $bundle_name, 0) == NULL) {
        $default_option = 'comment_body';
      }
      else {
        $default_option = variable_get('drupad_comment_field_' . $bundle_name, NULL);
      }

      // If the stored field has been deleted, we must reset the default value to nothing
      if (!isset($instances[$default_option])) {
        $default_option = NULL;
      }

      $form['comments_options']['drupad_comment_field_' . $bundle_name] = array(
        '#type' => 'select',
        '#title' => t('@bundle field', array('@bundle' => $bundle_info['label'])),
        '#options' => $options,
        '#disabled' => count($options) == 0 ? TRUE : FALSE,
        '#default_value' => $default_option,
        '#description' => t('Set the field to use as the <em>comment message</em> when posting a comment from the app. Note that only textarea field types are handled and thus listed.'),
      );
    }
  }

  // User permissions
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other settings'),
    '#collapsible' => TRUE,
  );

  $content_types = node_type_get_types($node = NULL, $reset = FALSE);

  $types = array();
  foreach ($content_types as $system_name => $content_type) {
    $types[$system_name] = $content_type->name;
  }

  asort($types, $sort_flags = SORT_STRING);

  $form['options']['drupad_content_types_excluded'] = array(
    '#type' => 'select',
    '#title' => t('Content types to exclude from content listing'),
    '#options' => $types,
    '#default_value' => variable_get('drupad_content_types_excluded', array()),
    '#description' => t('Select content types that you do not want Drupad to expose to the application, good for micro blogging streams. This setting does not affect node creation/editing'),
    '#multiple' => TRUE,
  );

  $form['options']['drupad_content_types_excluded_node_add'] = array(
    '#type' => 'select',
    '#title' => t('Content types to exclude from <em>Create content</em>'),
    '#options' => $types,
    '#default_value' => variable_get('drupad_content_types_excluded_node_add', array()),
    '#description' => t('Content types that shouldn\'t appear in <em>Create content</em>. Useful for <em>Panel</em>, <em>Webform</em>, etc.'),
    '#multiple' => TRUE,
  );

  $form['testing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Testing / debug'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['testing']['test_login'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
  );

  $form['testing']['test_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
  );

  $form['testing']['test'] = array(
    '#type' => 'button',
    '#value' => t('Launch the test'),
    '#ajax' => array(
      'callback' => 'drupad_request_config_callback',
      'wrapper' => 'drupal-debug-result',
      'effect' => 'fade',
    ),
  );

  $form['testing']['wrapper'] = array(
    '#prefix' => '<div id="drupal-debug-result">',
    '#suffix' => '</div>',
  );

  $form = system_settings_form($form);

  $form['#submit'] = array('system_settings_form_submit', 'drupad_settings_form_submit');

  return $form;
}

function drupad_settings_form_submit($form, &$form_state) {
  menu_rebuild();
}