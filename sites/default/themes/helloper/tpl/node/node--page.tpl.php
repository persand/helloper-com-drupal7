<article id="article-<?php echo $node->nid; ?>" class="<?php echo $classes; ?>" <?php echo $attributes; ?> role="article">
  <div class="article-content"<?php echo $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      echo render($content);
    ?>
  </div>
</article>
<!-- / #article-<?php echo $node->nid; ?> -->