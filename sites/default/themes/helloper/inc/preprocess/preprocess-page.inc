<?php

if (arg(0) == 'user' && arg(1) == 'login') {
  drupal_set_title(t('User login'));
}

if (arg(0) == 'user' && arg(1) == 'password') {
  drupal_set_title(t('Request new password'));
}

if (arg(0) == 'user' && arg(1) == 'register') {
  drupal_set_title(t('Create new account'));
}

$vars['primary_local_tasks'] = menu_primary_local_tasks();
$vars['secondary_local_tasks'] = menu_secondary_local_tasks();
