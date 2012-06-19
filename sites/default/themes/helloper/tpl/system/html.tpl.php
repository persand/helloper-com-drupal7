<?php echo $doctype ?>
<html class="no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>>
<head<?php echo $rdf_profile ?>>
  <?php echo $head ?>
  <title><?php echo $head_title ?></title>
  <?php echo $styles ?>
  <script src="/<?php echo $theme_path ?>/js/vendor/modernizr.min.js"></script>
</head>
<body class="<?php echo $classes ?>" <?php echo $attributes ?>>
  <?php echo $page_top ?>

  <?php echo $page ?>

  <?php echo $scripts ?>

  <!--[if (lt IE 9) & (!IEMobile)]>
    <script src="/<?php echo $theme_path ?>/js/ie.js"></script>
  <![endif]-->

  <?php echo $page_bottom ?>
</body>
</html>