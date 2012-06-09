<?php
// $Id: views-view.tpl.php,v 1.13.4.4 2010/07/04 10:04:51 dereine Exp $
/**
 * @file views-view.tpl.php
 * Main view template
 *
 * Variables available:
 * - $classes_array: An array of classes determined in
 *   template_preprocess_views_view(). Default classes are:
 *     .view
 *     .view-[css_name]
 *     .view-id-[view_name]
 *     .view-display-id-[display_name]
 *     .view-dom-id-[dom_id]
 * - $classes: A string version of $classes_array for use in the class attribute
 * - $css_name: A css-safe version of the view name.
 * - $css_class: The user-specified classes names, if any
 * - $header: The view header
 * - $footer: The view footer
 * - $rows: The results of the view query, if any
 * - $empty: The empty text to display if the view is empty
 * - $pager: The pager next/prev links to display, if any
 * - $exposed: Exposed widget form/info to display
 * - $feed_icon: Feed icon to display, if any
 * - $more: A link to view more, if any
 * - $admin_links: A rendered list of administrative links
 * - $admin_links_raw: A list of administrative links suitable for theme('links')
 *
 * @ingroup views_templates
 */
?>
<section id="<?php echo $view_id ?>" class="<?php echo $classes ?>">
  <?php if (!empty($admin_links)): ?>
    <?php echo $admin_links ?>
  <?php endif; ?>
  
  <?php if (!empty($header)): ?>
    <header class="view-header">
      <?php echo $header ?>
    </header>
  <?php endif; ?>

  <?php if (!empty($exposed)): ?>
    <section class="view-filters" role="form">
      <?php echo $exposed ?>
    </section>
  <?php endif; ?>

  <?php if (!empty($attachment_before)): ?>
    <section class="attachment attachment-before">
      <?php echo $attachment_before ?>
    </section>
  <?php endif; ?>

  <?php if (!empty($rows)): ?>
    <section class="view-content">
      <?php echo $rows ?>
    </section>
  <?php elseif (!empty($empty)): ?>
    <section class="view-empty">
      <?php echo $empty ?>
    </section>
  <?php endif; ?>

  <?php if (!empty($pager)): ?>
    <nav class="pager"><?php echo $pager ?></nav>
  <?php endif; ?>

  <?php if (!empty($attachment_after)): ?>
    <section class="attachment attachment-after">
      <?php echo $attachment_after ?>
    </section>
  <?php endif; ?>

  <?php if (!empty($more)): ?>
      <?php echo $more ?>
  <?php endif; ?>

  <?php if (!empty($footer)): ?>
    <footer class="view-footer">
      <?php echo $footer ?>
    </footer>
  <?php endif; ?>

</section>
<!-- /#<?php echo $view_id ?> -->