<?php

/**
 * @file
 * Maintenance mode handler
 */

function drupad_offline() {
  if (!user_access('administer site configuration')) {
    return array();
  }

  if (arg(2)) {
    $switch_to = arg(2);
    if (isset($switch_to) && $switch_to !== 'offline' && $switch_to !== 'online') {
      drupal_not_found();
    }
  }

  // Keep track of old state
  $was = variable_get('maintenance_mode', 0);

  if (isset($switch_to) && $switch_to === 'offline') {
    variable_set('maintenance_mode', 1);
  }
  elseif (isset($switch_to) && $switch_to === 'online') {
    variable_set('maintenance_mode', 0);
  }

  // Get new state
  $is = variable_get('maintenance_mode', 0);

  $data['meta'] = array();

  $data['content'] = array(
    'offline_was' => $was,
    'offline_is' => $is,
  );

  drupad_serve_response($data);
}