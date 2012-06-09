<?php

/**
 * @file
 * Content creation / editing handler
 */

function drupad_content_forms() {
  switch (arg(2)) {
    // ?q=drupad/content-forms/node/[nid]
    case 'node':
      module_load_include('inc', 'drupad', 'drupad.forms');
      $data = drupad_content_get_form_for_node(arg(3));
      break;
    // ?q=drupad/content-forms/node-add/[content_type]
    case 'node-add':
      module_load_include('inc', 'drupad', 'drupad.forms');
      $data = drupad_content_get_form_for_node_type(arg(3));
      break;
    // ?q=drupad/content-forms
    default:
      $data = drupad_content_get_content_types();
      break;
  }
  drupad_serve_response($data);
}

/**
 * Get all content types the current user can create
 *
 * @return structured response array
 */
function drupad_content_get_content_types() {
  $types = node_type_get_types('types', NULL, FALSE);

  $content_types = array();
  $content_types_to_exclude = variable_get('drupad_content_types_excluded_node_add', array());

  foreach ($types as $key => $type) {
    if (node_access('create', $type->type) && !in_array($type->type, $content_types_to_exclude)) {
      $type_to_add = new stdClass();
      $type_to_add->type = $type->type;
      $type_to_add->name = $type->name;
      $type_to_add->description = $type->description;
      $content_types[] = $type_to_add;
    }
  }

  $data['meta'] = array();
  $data['content'] = $content_types;

  return $data;
}

/**
 * We need to overwrite node_form_submit() to redirect node form
 * submissions to our own node view callback once submission succeed
 *
 * @see drupad_node_form_submit()
 */
function drupad_form_node_form_alter(&$form, $form_state, $form_id) {
  // We only alter forms when the iOS app is requesting a node form
  if (arg(0) == 'drupad' && arg(1) == 'content-forms' && isset($form['#node']->type) && $form_id == $form['#node']->type . '_node_form') {
    $form['actions']['submit']['#submit'][] = 'drupad_node_form_submit';
    // Vertical tabs are evil, they made me crazy for a whole day
    if (isset($form['additional_settings']) && $form['additional_settings']['#type'] == 'vertical_tabs') {
      unset($form['additional_settings']);
    }
    // Redirect.module kills us by using #rows for generating a table via the FAPI
    // where #rows it is intended to be an integer for textarea form component
    // @todo contact the author of the module
    if (module_exists('redirect') && isset($form['redirect']) && isset($form['redirect']['#title']) && $form['redirect']['#title'] == t('URL redirects')) {
      unset($form['redirect']);
    }
  }
}

/**
 * Set redirection of the node form
 *
 * @see node_form_submit()
 * @see drupad_form_alter()
 */
function drupad_node_form_submit($form, &$form_state) {
  // Redirect to the node edit form if it has succeed at saving
  if (isset($form_state['nid'])) {
    $form_state['redirect'] = 'drupad/content-forms/node/' . $form_state['nid'];
  }
}

function drupad_content_get_form_for_node_type($content_type) {
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'node', 'content_types');
  $data = array();

  // We need to build the form before getting Errors information
  $data['content'] = drupad_node_add($content_type);
  $data['meta'] = array();
  $data['meta']['errors'] = form_get_errors() ? form_get_errors() : array();

  return $data;
}

function drupad_content_get_form_for_node($nid) {
  module_load_include('inc', 'node', 'node.pages');
  $data = array('meta' => array());

  $node = node_load($nid);

  if (node_access('update', $node, NULL)) {
    // We need to build the form before getting Errors information
    $data['content'] = drupad_node_add($node->type, $node);
    $data['meta']['errors'] = form_get_errors() ? form_get_errors() : array();
  }
  else {
    $data['content'] = array();
    $data['meta']['errors'] = array('node_access' => FALSE);
    watchdog('access denied', '@q accessed via Drupad iOS app', array('@q' => $_GET['q']), WATCHDOG_WARNING);
  }

  return $data;
}

/**
 * Returns a form structure either for a node type or a node
 *
 * @param $type
 *  the content type to retrieve
 * @param $node
 *  the node that should fill the form (can be NULL)
 *
 * @return either a form structured array or FALSE
 */
function drupad_node_add($type, $node = NULL) {
  global $user;

  $types      = node_type_get_types();
  $type       = isset($type) ? str_replace('-', '_', $type) : NULL;
  $form_id    = $type . '_node_form';
  $form_state = array();

  // If a node type has been specified, validate its existence.
  if (isset($types[$type]) && node_access('create', $type)) {
    // Initialize settings:
    if (!$node) {
      $node = array(
        'uid' => $user->uid,
        'name' => (isset($user->name) ? $user->name : ''),
        'type' => $type,
        'language' => LANGUAGE_NONE,
      );
    }

    $form = drupal_get_form($type . '_node_form', (object) $node, array('no_cache' => TRUE));

    drupad_form_apply_pre_render($form);

    // I should really optimise this code, looping 3 times recursively
    // isn't effective at all - but for now - it works. It's 'make it work' time
    // 'Optimization time' will come... :-)
    drupad_convert_form_options_to_strict_array($form);
    drupad_convert_form_files_uri_to_url($form);
    drupad_remove_form_groups_recursion($form);

    return $form;
  }

  return FALSE;
}