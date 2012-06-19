<?php

/**
 * Implementation of hook_css_alter()
 *
 * Cherry pick CSS files and load them properly into on file.
 */
function helloper_css_alter(&$css) {
  $theme_path = drupal_get_path('theme', 'helloper');

  // Exclude certain files
  // Credits: http://drupal.org/project/tao
  $exclude = array(
    'modules/field/theme/field.css' => FALSE,
    'modules/node/node.css' => FALSE,
    'modules/system/system.menus.css' => FALSE,
    'modules/system/system.messages.css' => FALSE,
    'modules/system/system.theme.css' => FALSE,
    'modules/user/user.css' => FALSE,
    'sites/all/modules/date/date_api/date.css' => FALSE,
    'sites/all/modules/date/date_popup/themes/datepicker.1.7.css' => FALSE,
    'sites/all/modules/date/date_repeat_field/date_repeat_field.css' => FALSE,
    'sites/all/modules/oembed/oembed.base.css' => FALSE,
    'sites/all/modules/oembed/oembed.theme.css' => FALSE,
    'sites/all/modules/views/css/views.css' => FALSE,
  );
  $css = array_diff_key($css, $exclude);

  foreach (array_keys($css) as $filename) {
    // Apply the same settings to all files to combine them to just one file
    // when CSS aggregation is on.
    $css[$filename]['media'] = 'all';
    $css[$filename]['group'] = 0;
    $css[$filename]['every_page'] = 1;

    // Load core and contrib files before those in my theme
    if (strpos($filename, $theme_path) === false) {
      $css[$filename]['weight'] = $css[$filename]['weight'] - 1000;
    }
  }
}

/**
 * Implementation of hook_js_alter()
 *
 * Load them properly into on file. 
 */
function helloper_js_alter(&$javascript) {
  // Get path to my theme
  $theme_path = drupal_get_path('theme', 'helloper');
  
  foreach($javascript as $script => $js) {
    // Skip the settings
    if ($script == 'settings') continue;

    // Apply the same settings to all files to combine them to just one file
    // when JS aggregation is on.
    $javascript[$script]['every_page'] = 1;
    $javascript[$script]['group'] = 1;

    // Load files from my theme last and in the same order as in the theme .info
    if (stristr($script, $theme_path)) $javascript[$script]['weight'] = $javascript[$script]['weight'] * 100 + 50;
  }
}