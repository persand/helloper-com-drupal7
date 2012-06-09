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

// Mixpanel Flow
$vars['page_bottom'] .= "\n".'
<!-- start Mixpanel --><script type="text/javascript">(function(d,c){var a,b,g,e;a=d.createElement("script");a.type="text/javascript";a.async=!0;a.src=("https:"===d.location.protocol?"https:":"http:")+\'//api.mixpanel.com/site_media/js/api/mixpanel.2.js\';b=d.getElementsByTagName("script")[0];b.parentNode.insertBefore(a,b);c._i=[];c.init=function(a,d,f){var b=c;"undefined"!==typeof f?b=c[f]=[]:f="mixpanel";g="disable track track_pageview track_links track_forms register register_once unregister identify name_tag set_config".split(" ");
for(e=0;e<g.length;e++)(function(a){b[a]=function(){b.push([a].concat(Array.prototype.slice.call(arguments,0)))}})(g[e]);c._i.push([a,d,f])};window.mixpanel=c})(document,[]);
mixpanel.init("2622507bdf0bea0ad966ba92c55ec5a9");</script><!-- end Mixpanel -->
';
