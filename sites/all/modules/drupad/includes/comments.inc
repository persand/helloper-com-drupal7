<?php

/**
 * @file
 * Comments handler
 */

function drupad_comments() {
  // Switch on the verb/action to take
  switch (arg(3)) {
    case 'publish':
      $data = drupad_comments_publish_cid(arg(2));
      break;
    case 'unpublish':
      $data = drupad_comments_unpublish_cid(arg(2));
      break;
    case 'report-mollom-delete':
      $data = drupad_comments_report_and_delete_cid(arg(2));
      break;
    case 'report-mollom-unpublish':
      $data = drupad_comments_report_and_unpublish_cid(arg(2));
      break;
    case 'delete':
      $data = drupad_comments_delete_cid(arg(2));
      break;
    case 'reply':
      $data = drupad_comments_reply_cid(arg(2));
      break;
    case 'node':
      $data = drupad_comments_post_to_nid(arg(2));
      break;
    default:
      $data = drupad_comments_list();
      break;
  }
  drupad_serve_response($data);
}

function drupad_comments_list() {
  if (!user_access('access comments')) {
    return array();
  }

  global $pager_total_items;

  if (isset($_GET['status']) && $_GET['status'] != '' && is_numeric($_GET['status'])) {
    $status = _drupad_fix_comment_status($_GET['status']);
  }
  else {
    $status = COMMENT_PUBLISHED;
  }

  $comments = array();

  $query = db_select('comment', 'c')->extend('PagerDefault');
  $query->fields('c', array('cid'));
  $query->condition('c.status', $status, '=');
  $query->condition('c.nid', 0, '<>');
  $query->orderBy('c.created', 'DESC');

  $result = $query->execute();

  $cids = array();

  foreach ($result as $row) {
    $cids[] = $row->cid;
  }

  $db_comments = comment_load_multiple($cids);
  $comments = array();

  foreach ($db_comments as $db_comment) {
    $comment = new stdClass();
    $comment->subject = $db_comment->subject;
    $comment->cid = $db_comment->cid;
    $comment->nid = $db_comment->nid;
    $comment->uid = $db_comment->uid;
    $comment->timestamp = $db_comment->created;
    $comment->status = $db_comment->status == 0 ? 1 : 0; // We inverse the status because the app was designed against D6
    $comment->name = $db_comment->name;
    $comment->homepage = $db_comment->homepage;

    // Grab user details
    $account = user_load($db_comment->uid);
    $comment->registered_name = $account->name != '' ? $account->name : t(variable_get('anonymous', 'Anonymous'));
    $comment->mail = $account->mail;

    if (!is_null($account->picture) && !is_null($account->picture->uri)) {
      $comment->picture = file_create_url($account->picture->uri);
    }
    else {
      $comment->picture = NULL;
    }

    // Grab node related details
    $node = node_load($db_comment->nid);
    $comment->node_title = $node->title;
    $comment->node_url = url('node/' . $comment->nid, array('absolute' => TRUE));
    $comment->node_comment_mode = (int) $node->comment;
    $comment->comment_url = url('node/' . $comment->nid, array('absolute' => TRUE, 'fragment' => "comment-{$comment->cid}"));

    // Prepare the comment array for rendering
    $rendering_comment = comment_view($db_comment, $node, 'full');
    // But we remove contextual links
    unset($rendering_comment['links']);
    $comment->comment = drupal_render($rendering_comment);
    $comments[] = $comment;
  }

  $data['content'] = $comments;

  $data['meta']['total_count'] = $pager_total_items[0];
  $data['meta']['type'] = 'comments';
  return $data;
}

function drupad_comments_publish_cid($cid) {
  if (!user_access('administer comments')) {
    return array();
  }

  $comment = comment_load($cid);
  $comment->status = COMMENT_PUBLISHED;

  comment_save($comment);
  $comment = comment_load($cid);
  watchdog('action', 'Published comment %subject.', array('%subject' => $comment->subject));

  $data = array(
    'meta' => array(
      'cid' => $cid,
      'change' => 'status',
      'value' => 0, // We should use COMMENT_PUBLISHED, but the app speaks D6 for now
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_comments_unpublish_cid($cid) {
  if (!user_access('administer comments')) {
    return array();
  }

  $comment = comment_load($cid);
  $comment->status = COMMENT_NOT_PUBLISHED;

  comment_save($comment);
  watchdog('action', 'Unpublished comment %subject.', array('%subject' => $comment->subject));
  $comment = comment_load($cid);

  $data = array(
    'meta' => array(
      'cid' => $cid,
      'change' => 'status',
      'value' => 1, // We should use COMMENT_NOT_PUBLISHED, but the app speaks D6 for now
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_comments_delete_cid($cid) {
  if (!user_access('administer comments')) {
    return array();
  }

  $comment = comment_load($cid);
  module_load_include('inc', 'comment', 'comment.admin');
  // Delete comment and its replies.
  comment_delete($comment->cid);
  watchdog('content', 'Deleted comment @cid and its replies.', array('@cid' => $comment->cid));
  _comment_update_node_statistics($comment->nid);
  // Clear the cache so an anonymous user sees that his comment was deleted.
  cache_clear_all();
  $data = array(
    'meta' => array(
      'cid' => $cid,
      'change' => 'deleted',
      'value' => 1,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_comments_report_and_delete_cid($cid) {
  if (!user_access('administer comments')) {
    return array();
  }

  module_load_include('inc', 'comment', 'comment.admin');
  $comment = comment_load($cid);
  // First, report the comments as spam to Mollom.com.
  if (module_exists('mollom') && $data = mollom_data_load('comment', $cid)) {
    _mollom_send_feedback($data->session);
  }

  comment_delete($comment->cid);
  watchdog('content', 'Deleted comment @cid and its replies. Mollom was notified.', array('@cid' => $comment->cid));
  _comment_update_node_statistics($comment->nid);

  cache_clear_all();

  $data = array(
    'meta' => array(
      'nid' => $comment->nid,
      'change' => 'deleted',
      'value' => 1,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_comments_report_and_unpublish_cid($cid) {
  if (!user_access('administer comments')) {
    return array();
  }

  $comment = comment_load($cid);
  // First, report the comments as spam to Mollom.com.
  if (module_exists('mollom') && $data = mollom_data_load('comment', $cid)) {
    _mollom_send_feedback($data->session);
  }

  $comment->status = COMMENT_NOT_PUBLISHED;
  comment_save($comment);
  watchdog('action', 'Unpublished comment %subject.', array('%subject' => $comment->subject));
  _comment_update_node_statistics($comment->nid);

  cache_clear_all();

  $data = array(
    'meta' => array(
      'cid' => $cid,
      'change' => 'status',
      'value' => 1, // We should use COMMENT_NOT_PUBLISHED, but the app speaks D6 for now
    ),
    'content' => array(),
  );

  return $data;
}

function drupad_comments_reply_cid($cid) {
  if (!user_access('post comments') || !user_access('post comments without approval')) {
    return array();
  }

  // Find the node for which the comment is made
  $nid = db_query("SELECT nid FROM {comment} WHERE cid = :cid", array(':cid' => $cid))->fetchField();

  return _drupad_comments_insert($nid, $cid);
}

function drupad_comments_post_to_nid($nid) {
  if (!user_access('post comments') || !user_access('post comments without approval')) {
    return array();
  }

  return _drupad_comments_insert($nid);
}

function _drupad_comments_insert($nid = NULL, $pid = NULL) {
  global $user;

  // Validate the comment's subject. If not specified, extract
  // one from the comment's body.
  if (trim($_POST['subject']) == '') {
    // The body may be in any format, so we:
    // 1) Filter it into HTML
    // 2) Strip out all HTML tags
    // 3) Convert entities back to plain-text.
    // Note: format is checked by check_markup().
    $_POST['subject'] = truncate_utf8(trim(decode_entities(strip_tags(check_markup($_POST['comment'])))), 29, TRUE);
    // Edge cases where the comment body is populated only by HTML tags will
    // require a default subject.
    if ($_POST['subject'] == '') {
      $_POST['subject'] = t('(No subject)');
    }
  }

  $comment = new stdClass();
  $comment->nid = $nid;
  $comment->pid = $pid;
  $comment->cid = NULL;
  $comment->uid = $user->uid;
  $comment->language = $user->language;
  $comment->subject = $_POST['subject'];
  $comment->format = 0; // is it usefull?
  $comment->is_anonymous = FALSE;

  // Take care of the message field
  $node = node_load($nid);
  $node_type = node_type_get_type($node);
  $comment_field = variable_get('drupad_comment_field_comment_node_' . $node_type->type);

  $comment->{$comment_field}['und'][0]['value'] = $_POST['comment'];
  $comment->{$comment_field}['und'][0]['format'] = filter_fallback_format();

  $comment = comment_submit($comment);
  comment_save($comment);

  // Add an entry to the watchdog log.
  watchdog('content', 'Comment posted: %subject.', array('%subject' => $comment->subject), WATCHDOG_NOTICE, l(t('view'), 'comment/' . $comment->cid, array('fragment' => 'comment-' . $comment->cid)));

  $data = array(
    'meta' => array(
      'cid' => (int) $comment->cid,
      'change' => 'status',
      'value' => TRUE,
    ),
    'content' => array(),
  );

  return $data;
}

function _drupad_fix_comment_status($d6_status) {
  if ($d6_status == 1) {
    return COMMENT_NOT_PUBLISHED;
  }
  else {
    return COMMENT_PUBLISHED;
  }
}