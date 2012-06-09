<?php

/**
 * Implementation of hook_css_alter()
 *
 * Specify either disallowed or allowed CSS files.
 * If neither are set all CSS files will be used.
 */
function helloper_css_alter(&$css) {
  // Get path to my theme
  $theme_path = drupal_get_path('theme', 'helloper');
  
  // Set two empty arrays
  $disallowed_css = array();
  $allowed_css = array();
  
  // Disallow these CSS files – avoid CSS bloat
  $disallowed_css[] = 'modules/field/theme/field.css';
  $disallowed_css[] = 'modules/node/node.css';
  $disallowed_css[] = 'modules/system/system.menus.css';
  $disallowed_css[] = 'modules/system/system.messages.css';
  $disallowed_css[] = 'modules/system/system.theme.css';
  $disallowed_css[] = 'modules/user/user.css';
  $disallowed_css[] = 'sites/all/modules/date/date_api/date.css';
  $disallowed_css[] = 'sites/all/modules/date/date_popup/themes/datepicker.1.7.css';
  $disallowed_css[] = 'sites/all/modules/date/date_repeat_field/date_repeat_field.css';
  $disallowed_css[] = 'sites/all/modules/oembed/oembed.base.css';
  $disallowed_css[] = 'sites/all/modules/oembed/oembed.theme.css';
  $disallowed_css[] = 'sites/all/modules/views/css/views.css';
  
  // Loop CSS files
  foreach (array_keys($css) as $filename) {
    // Unset file? Disallowed precedes allowed
    if (!empty($disallowed_css)) {
      if (in_array($filename, $disallowed_css) && strpos($filename, $theme_path) === false) {
        unset($css[$filename]);
      }
    } elseif (!empty($allowed_css)) {
       if (!in_array($filename, $allowed_css) && strpos($filename, $theme_path) === false) {
        unset($css[$filename]);
      }
    }
    
    // CSS file is ok!
    if (!empty($css[$filename])) {
      // Apply the same settings to all files to combine them into just one
      // file when CSS aggregation is on.
      $css[$filename]['media'] = 'all';
      $css[$filename]['group'] = 0;
      $css[$filename]['every_page'] = 1;

      if ($filename == $theme_path.'/css/vendor/normalize.css') {
        $css[$filename]['weight'] = 0; // Load normalize.css first
      } elseif ($filename == $theme_path.'/css/globals.css') {
        $css[$filename]['weight'] = 0.00001; // Load setup.css second
      } elseif (strpos($filename, $theme_path) === false) {
        $css[$filename]['weight'] = 0.00011; // Load module CSS before theme
      } 
    }
  }
}

/**
 * Implementation of hook_js_alter()
 *
 * Remove some stuff and combine JS to just one file
 */
function helloper_js_alter(&$javascript) {
  // Get path to my theme
  $theme_path = drupal_get_path('theme', 'helloper');
  
  foreach($javascript as $script => $js) {
    // Skip the settings
    if ($script == 'settings') continue;

    // Load theme JS last and in the order specified in theme info file
    if (stristr($script, $theme_path)) $javascript[$script]['weight'] = $javascript[$script]['weight'] * 100 + 50;

    // Load all javascripts on every page and within the same group
    // This results in just one JS file
    $javascript[$script]['every_page'] = 1;
    $javascript[$script]['group'] = 1;
  }
}