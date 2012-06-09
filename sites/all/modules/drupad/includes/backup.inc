<?php

/**
 * @file
 * Backup and migrate handler
 */

function drupad_backup() {
  if (!user_access('perform backup')) {
    return array();
  }

  $source_id = 'db';
  $destination_id = 'manual';
  $profile_id = 'default';

  backup_migrate_include('profiles', 'destinations');

  // Set the message mode to logging.
  //_backup_migrate_message_callback('_backup_migrate_message_drush');

  if (!backup_migrate_get_destination($source_id)) {
    _backup_migrate_message("Could not find the source '@source'. Try using 'drush bam sources' to get a list of available sources or use 'db' to backup the Drupal database.", array('@source' => $source_id), 'error');
    return;
  }
  if (!backup_migrate_get_destination($destination_id)) {
    _backup_migrate_message("Could not find the destination '@destination'. Try using 'drush bam destinations' to get a list of available destinations.", array('@destination' => $destination_id), 'error');
    return;
  }
  $settings = backup_migrate_get_profile($profile_id);
  if (!$settings) {
    _backup_migrate_message("Could not find the profile '@profile'. Try using 'drush bam profiles' to get a list of available profiles.", array('@profile' => $profile_id), 'error');
    return;
  }

  _backup_migrate_message('Starting backup...');

  $settings->destination_id = $destination_id;
  $settings->source_id = $source_id;

  $data = array(
    'meta' => array(),
    'content' => backup_migrate_perform_backup($settings),
  );

  drupad_serve_response($data);
}