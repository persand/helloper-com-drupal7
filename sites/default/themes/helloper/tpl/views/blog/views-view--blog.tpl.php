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
      <?php echo $rows ?>
  <?php elseif (!empty($empty)): ?>
    <section class="view-empty">
      <?php echo $empty ?>
    </section>
  <?php endif; ?>

  <?php if (!empty($pager)): ?>
    <div class="pager"><?php echo $pager ?></div>
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