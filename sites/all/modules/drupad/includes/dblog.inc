<?php

/**
 * @file
 * Database logging handler
 */

function drupad_dblog() {
  if (!user_access('access site reports')) {
    return array();
  }

  $query = db_select('watchdog', 'w')->extend('PagerDefault');
  $query->fields('w')->orderBy('timestamp', 'DESC')->limit(50);

  $result = $query->execute();

  foreach ($result as $message) {
    if ($message->variables !== 'N;') {
      $message->message = t($message->message, unserialize($message->variables));
    }
    unset($message->variables);
    $message->wid = (int) $message->wid;
    $message->uid = (int) $message->uid;
    $message->timestamp = (int) $message->timestamp;
    $message->severity = (int) $message->severity;

    $account = user_load($message->uid);
    $message->username =  $account->name ? $account->name : t(variable_get('anonymous', 'Anonymous'));
    $message->mail = $account->mail ? $account->mail : NULL;

    if (!is_null($account->picture) && !is_null($account->picture->uri)) {
      $message->picture = file_create_url($account->picture->uri);
    }
    else {
      $message->picture = NULL;
    }

    $messages[] = $message;
  }

  $datas['content'] = $messages;

  global $pager_total_items;
  $datas['meta']['total_count'] = $pager_total_items[0];

  drupad_serve_response($datas);
}
