<?php

if (empty($vars['head'])) {
  $vars['head'] = '';
}

$vars['head'] .= '<script type="text/javascript" src="http://use.typekit.com/yne8giv.js"></script><script type="text/javascript">try{Typekit.load();}catch(e){}</script>';

// Force IE to use chrome JS if available
$vars['head'] .= "\n".'<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">';

// Force Microsoft to use Cleartype
$vars['head'] .= "\n".'<meta http-equiv="cleartype" content="on">';

$vars['head'] .= "\n".'<link rel="license" href="http://creativecommons.org/licenses/by/3.0/">';

$vars['head'] .= "\n".'<link rel="alternate" type="application/rss+xml" title="Feed" href="http://feeds.feedburner.com/HelloPer" />';

// Mobile viewport
$vars['head'] .= "\n".'<meta name="viewport" content="width=device-width">';

// Hide Mobile Safari UI
// Documentation: http://bit.ly/appletouchicon
$vars['head'] .= "\n".'<meta name="apple-mobile-web-app-capable" content="yes">';

// Changing the Mobile Safari Status Bar Appearance
// Documentation: http://bit.ly/appletouchicon
$vars['head'] .= "\n".'<meta name="apple-mobile-web-app-status-bar-style" content="black">';

// Clean up />
$vars['head'] = str_replace(' />', '>', $vars['head']);

// Change name of the node type class
// Credit: Adaptivetheme by Jeff Burnz
$classes = explode(' ', $vars['classes']);
if ($node = menu_get_object()) {
  $node_type_class = drupal_html_class('node-type-' . $node->type);
  if (in_array($node_type_class, $classes)) {
    theme_get_setting('extra_page_classes') == 0 ? '' : $classes = str_replace($node_type_class, '', $classes);
    $classes = str_replace('node-type-', 'page-type-', $classes);
  }
}
$vars['classes'] = trim(implode(' ', $classes));

// Remove Drupals built in "no-sidebars class"
$vars['classes'] = str_replace(' no-sidebars', '', $vars['classes']);

if (empty($vars['page_bottom'])) {
  $vars['page_bottom'] = '';
}