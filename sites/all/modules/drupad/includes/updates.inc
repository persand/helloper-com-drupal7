<?php

/**
 * @file
 * Projects updates handler
 */

function drupad_updates() {
  if (!user_access('administer site configuration')) {
    return array();
  }

  if ($available = update_get_available(TRUE)) {
    module_load_include('inc', 'update', 'update.compare');
    $projects = update_calculate_project_data($available);

    $data = array();
    $datas = array();
    foreach ($projects as $key => $project) {
      $data['system_name'] = $project['name'];
      $data['name'] = isset($project['title']) ? $project['title'] : $project['info']['name'];
      $data['version'] = $project['info']['version'];
      $data['status'] = $project['status'];
      $datas['content'][$project['project_type']][] = $data;
    }

    $datas['meta']['projects_types_order'] = array('core', 'module', 'theme');

    drupad_serve_response($datas);
  }
}