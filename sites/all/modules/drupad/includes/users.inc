<?php

/**
 * @file
 * Users handler
 */

function drupad_users() {
  switch (arg(3)) {
    // ?q=drupad/users/[uid]/block
    case 'block':
      $data = drupad_users_block_uid(arg(2));
      break;
    // ?q=drupad/users/[uid]/active
    case 'active':
      $data = drupad_users_active_uid(arg(2));
      break;
    // ?q=drupad/users/[uid]/delete
    case 'delete':
      $data = drupad_users_delete_uid(arg(2));
      break;
    // ?q=drupad/users/[uid]/edit
    case 'edit':
      module_load_include('inc', 'drupad', 'drupad.forms');
      $data = drupad_users_edit_uid(arg(2));
      break;
    // ?q=drupad/users/[login]/get
    case 'get':
      $data = drupad_user_get(arg(2));
      break;
    // ?q=drupad/users
    default:
      $data = drupad_users_list();
      break;
  }

  drupad_serve_response($data);
}

function drupad_users_list() {
  if (!user_access('administer users')) {
    return array();
  }

  if (isset($_GET['status']) && $_GET['status'] != '' && is_numeric($_GET['status'])) {
    $status = (int) $_GET['status'];
  }
  else {
    $status = 1;
  }

  $users = array();

  $query = db_select('users', 'u')->extend('PagerDefault');
  $query->condition('u.uid', 0, '<>')
        ->fields('u', array('uid', 'name', 'status', 'mail', 'picture', 'created', 'login'))
        ->orderBy('created', 'DESC')
        ->limit(50);

  $result = $query->execute();

  $users = array();

  foreach ($result as $user) {
    if (!is_null($user->picture)) {
      $file = file_load($user->picture);
      $user->picture = isset($file->uri) ? file_create_url($file->uri) : NULL;
    }
    $users[] = $user;
  }

  $data['content'] = $users;

  global $pager_total_items;
  $data['meta']['total_count'] = $pager_total_items[0];
  return $data;
}

function drupad_user_get($username) {
  if (!user_access('administer users')) {
    return array();
  }

  $users = array();

  $query = db_select('users', 'u');
  $query->condition('u.name', $username, '=')
        ->fields('u', array('uid', 'name', 'status', 'mail', 'picture', 'created', 'login'));

  $result = $query->execute();

  foreach ($result as $user) {
    if (!is_null($user->picture)) {
      $file = file_load($user->picture);
      $user->picture = isset($file->uri) ? file_create_url($file->uri) : NULL;
    }
    $users[] = $user;
  }

  $data['content'] = $users;

  $data['meta'] = array('username' => $username);
  return $data;
}

function drupad_users_block_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1 || $uid == 0) {
    $data = array(
      'meta' => array('uid' => $uid, 'was_blocked' => FALSE),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);
  // Skip blocking user if they are already blocked.
  if ($account !== FALSE && $account->status == 1) {
    user_save($account, array('status' => 0));
  }
  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'status',
      'value' => 0,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_users_active_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1) {
    $data = array(
      'meta' => array('uid' => $uid, 'was_actived' => FALSE),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);
  // Skip blocking user if they are already blocked.
  if ($account !== FALSE && $account->status == 0) {
    user_save($account, array('status' => 1));
  }
  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'status',
      'value' => 1,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_users_delete_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1 || $uid == 0) {
    $data = array(
      'meta' => array(
        'uid' => $uid,
        'change' => 'delete',
        'value' => 0,
      ),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);

  if ($account !== FALSE) {
    user_delete($uid);
  }

  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'delete',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}

/**
 * Get a user profile form from a uid
 */
function drupad_users_edit_uid($uid) {
  module_load_include('inc', 'user', 'user.pages');
  $account = user_load($uid);

  $form = drupal_get_form('user_profile_form', $account, $category = 'account');

  drupad_form_apply_pre_render($form);

  // Recursion and JSON are not friends
  if (isset($form['#user']->uri)) {
    unset($form['#user']->uri);
  }

  // It is unlikely you'll want to edit a user timezone and in regards of the amount
  // of bytes it occupies, we remove it for iOS device
  if (isset($form['timezone'])) {
    unset($form['timezone']);
  }

  // I should really optimise this code, looping 3 times recursively
  // isn't effective at all - but for now - it works. It's 'make it work' time
  // 'Optimization time' will come... :-)
  drupad_convert_form_options_to_strict_array($form);
  drupad_convert_form_files_uri_to_url($form);
  drupad_remove_form_groups_recursion($form);

  $data = array(
    'meta' => array(),
    'content' => $form,
  );

  $data['meta']['errors'] = form_get_errors() ? form_get_errors() : array();
  return $data;
}