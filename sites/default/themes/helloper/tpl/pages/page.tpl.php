<?php
// $Id: page.tpl.php,v 1.14.2.10 2009/11/05 14:26:26 johnalbin Exp $

/**
 * @file page.tpl.php
 *
 * Theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template normally located in the
 * modules/system folder.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 * - $hide_site_name: TRUE if the site name has been toggled off on the theme
 *   settings page. If hidden, the "element-invisible" class is added to make
 *   the site name visually hidden, but still accessible.
 * - $hide_site_slogan: TRUE if the site slogan has been toggled off on the
 *   theme settings page. If hidden, the "element-invisible" class is added to
 *   make the site slogan visually hidden, but still accessible.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['header']: Items for the header region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the left sidebar.
 * - $page['sidebar_second']: Items for the right sidebar.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see kollegorna_process_page()
 */
?>
<?php if (!empty($messages)): echo '<section id="messages" role="alert">' . $messages . '</section>'; endif; ?>
  <?php if (($is_front && !$front_pager) || !empty($title)): ?>
    <header class="header row" role="header">
      <div class="columns eight push-two">
      <?php if ($is_front): ?>
        <hgroup class="headline">
          <a href="/about">
            <h1>Hello, I'm Per.</h1>
            <div><h2>I design and develop for the web.</h2></div>
          </a>
        </hgroup>
      <?php else: ?>
        <h1><?php echo $title; ?></h1>        
      <?php endif; ?>
      </div>
      <!-- / .columns six push-two -->
    </header>
    <!-- / .header -->
  <?php endif; ?>

  <section class="main row" role="main">
    <?php if ($primary_local_tasks || $secondary_local_tasks || $action_links): ?>
      <nav class="tasks" role="navigation">
        <?php if (!empty($primary_local_tasks)): ?>
          <ul class="tabs primary"><?php print render($primary_local_tasks); ?></ul>
        <?php endif; ?>
        <?php if (!empty($secondary_local_tasks)): ?>
          <ul class="tabs secondary"><?php print render($secondary_local_tasks); ?></ul>
        <?php endif; ?>
        <?php if (!empty($action_links)): ?>
          <ul class="action-links"><?php print render($action_links); ?></ul>
        <?php endif; ?>
      </nav>
      <!-- / .tasks -->
    <?php endif; ?>

    <div class="columns eight push-two"><?php echo render($page['content']); ?></div>
  </section>
  <!-- / .main, role="main" -->

  <?php if (!empty($page['footer'])): ?>
    <footer class="footer row" role="contentinfo">
      <div class="columns eight push-two"><?php echo render($page['footer']); ?></div>
    </footer>
    <!-- / .footer -->
  <?php endif; ?>

