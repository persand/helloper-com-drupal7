<?php

/**
 * @file
 * Status report handler
 */

function drupad_status_report() {
  if (!user_access('administer site configuration')) {
    return array();
  }

  $data = array();

  // Load .install files
  include_once './includes/install.inc';
  drupal_load_updates();

  // Check run-time requirements and status information.
  $requirements = module_invoke_all('requirements', 'runtime');
  usort($requirements, '_system_sort_requirements');

  foreach ($requirements as $key => $requirement) {
    $requirements[$key]['value'] = strip_tags($requirement['value']);
  }

  $data['meta'] = array();
  $data['content'] = $requirements;

  drupad_serve_response($data);
}