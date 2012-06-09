<?php

/**
 * @file
 * Cache handler
 */

function drupad_cache() {
  if (!user_access('administer site configuration')) {
    return array();
  }

  $run = FALSE;

  if (arg(2) == 'flush') {
    // Flush all caches; no need to re-implement this.
    module_load_include('inc', 'system', 'system.admin');
    $form = $form_state = array();
    system_clear_cache_submit($form, $form_state);
    $run = TRUE;
    watchdog('drupad', 'Cache has been flushed from Drupad', array(), WATCHDOG_INFO, NULL);
  }

  $data['meta'] = array();
  $data['content']['was_just_run'] = $run;
  drupad_serve_response($data);
}