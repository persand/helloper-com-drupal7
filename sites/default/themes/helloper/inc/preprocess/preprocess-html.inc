<?php
$vars['theme_path'] = drupal_get_path('theme', 'helloper');

if (module_exists('rdf')) {
  $vars['doctype'] = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML+RDFa 1.1//EN">' . "\n";
  $vars['rdf_version'] = ' version="HTML+RDFa 1.1"';
  $vars['rdf_profile'] = ' profile="' . $vars['grddl_profile'] . '"';
}
else {
  $vars['doctype'] = '<!DOCTYPE html>' . "\n";
  $vars['rdf_version'] = '';
  $vars['rdf_profile'] = '';
}

if (!empty($vars['page']['sidebar'])) {
	$vars['classes_array'][] = 'sidebar';
}