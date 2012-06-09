<?php

/**
 * @file
 * Content listing handler
 */

function drupad_content() {

  // Switch on the verb/action to take
  switch (arg(3)) {
    case 'publish':
      $data = drupad_content_publish_nid(arg(2));
      break;
    case 'unpublish':
      $data = drupad_content_unpublish_nid(arg(2));
      break;
    case 'report-mollom-delete':
      $data = drupad_content_report_and_delete_nid(arg(2));
      break;
    case 'report-mollom-unpublish':
      $data = drupad_content_report_and_unpublish_nid(arg(2));
      break;
    case 'delete':
      $data = drupad_content_delete_nid(arg(2));
      break;
    case 'promote':
      $data = drupad_content_promote_nid(arg(2));
      break;
    case 'unpromote':
      $data = drupad_content_unpromote_nid(arg(2));
      break;
    case 'sticky':
      $data = drupad_content_sticky_nid(arg(2));
      break;
    case 'unsticky':
      $data = drupad_content_unsticky_nid(arg(2));
      break;
    default:
      $data = drupad_content_list();
      break;
  }
  drupad_serve_response($data);
}

function drupad_content_list() {

  if (isset($_GET['status']) && $_GET['status'] != '' && is_numeric($_GET['status'])) {
    $status = (int) $_GET['status'];
  }
  else {
    $status = 1;
  }

  $nodes = array();

  $content_types = variable_get('drupad_content_types_excluded', array());

  $content_types = array_keys($content_types);

  $query = db_select('node', 'n')->extend('PagerDefault');
  $query->fields('n');
  $query->condition('n.status', $status, '=');

  // Excluded content types
  if (!empty($content_types)) {
    $query->condition('n.type', $content_types, 'NOT IN');
  }

  $query->innerJoin('users', 'u', 'n.uid = u.uid');
  $query->innerJoin('node_type', 'nt', 'n.type = nt.type');
  $query->innerJoin('node_revision', 'r', 'n.vid = r.vid');
  $query->addField('u', 'name', 'username');
  $query->addField('u', 'picture', 'fid');
  $query->addField('u', 'mail');
  $query->addField('nt', 'name', 'content_type');
  $query->limit(50);
  $query->orderBy('n.changed', 'DESC');
  $result = $query->execute();

  $nodes = array();

  foreach ($result as $node) {
    // Take care of user name
    $node->username = $node->username != '' ? $node->username : t('Anonymous');

    if ($node->fid > 0) {
      $node->picture = db_query("SELECT uri FROM {file_managed} WHERE fid = :fid", array(':fid' => $node->fid))->fetchObject()->uri;
    }
    unset($node->fid);

    // Make picture URI an absolute URL
    if (isset($node->picture) && !is_null($node->picture)) {
      $node->picture = file_create_url($node->picture);
    }

    $node_object = node_load($node->nid);
    $rendering_node = node_view($node_object, 'full');
    unset($rendering_node['links'], $rendering_node['#contextual_links']);

    $node->teaser = drupal_render($rendering_node);

    // Locale module support
    if (module_exists('locale')) {
      $language_full = db_query("SELECT name FROM {languages} WHERE language = :code", array(':code' => $node->language))->fetchField();
      $node->language_full =  $language_full != FALSE ? $language_full : "";
    }

    // Can the user view, edit or delete this node?
    $node->can_view   = node_access('view',   $node_object, NULL);
    $node->can_edit   = node_access('update', $node_object, NULL);
    $node->can_delete = node_access('delete', $node_object, NULL);

    $nodes[] = $node;
  }

  $data['content'] = $nodes;

  global $pager_total_items;
  $data['meta']['total_count'] = $pager_total_items[0];

  return $data;
}

function drupad_content_publish_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->status = 1;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'status',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_unpublish_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->status = 0;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'status',
      'value' => 0,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_report_and_delete_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  if (module_exists('mollom') && $data = mollom_data_load('node', $nid)) {
    _mollom_send_feedback($data->session);
  }

  node_delete($nid);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'deleted',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_report_and_unpublish_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  if (module_exists('mollom') && $data = mollom_data_load('node', $nid)) {
    _mollom_send_feedback($data->session);
  }

  $node = node_load($nid);

  $node->status = 0;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'status',
      'value' => 0,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_delete_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  node_delete($nid);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'deleted',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_promote_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->promote = 1;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'promote',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_unpromote_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->promote = 0;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'promote',
      'value' => 0,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_sticky_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->sticky = 1;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'sticky',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_content_unsticky_nid($nid) {
  if (!user_access('administer nodes')) {
    return array();
  }

  $node = node_load($nid);
  $node->sticky = 0;
  node_save($node);

  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array('@type' => $node->type, '%title' => $node->title);
  watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);

  $data = array(
    'meta' => array(
      'nid' => $nid,
      'change' => 'sticky',
      'value' => 0,
    ),
    'content' => array(),
  );

  return $data;
}