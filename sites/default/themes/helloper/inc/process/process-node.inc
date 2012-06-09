<?php
// Use article in class names instead of node
// Credit: Adaptivetheme by Jeff Burnz
$classes = explode(' ', $vars['classes']);
$classes = str_replace('node', 'article', $classes);
if (in_array('article-' . $vars['node']->type, $classes)) {
  $classes = str_replace('article-' . $vars['node']->type, 'article-type-' . $vars['node']->type, $classes);
}
$vars['classes'] = trim(implode(' ', $classes));

$vars['submitted'] = date('Y-m-d H:i', $vars['created']);
$vars['submitted_datetime'] = date('c', $vars['created']);
$vars['submitted_date'] = _helloper_relative_date($vars['created']);
$vars['submitted_time'] = date('H:i', $vars['created']);
$vars['stardate'] = _helloper_stardate($vars['created']);


switch ($vars['type']) {
  case 'note':
    if (!empty($vars['field_link'][und][0])) {
      $vars['node_url'] = $vars['field_link'][und][0]['url'];
    }
    break;
}