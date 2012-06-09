<?php echo $doctype ?>
<!--[if IEMobile 7 ]><html class="iem7 no-js" manifest="default.appcache?v=1"><![endif]-->
<!--[if lt IE 7 ]> <html class="ie6 no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>> <![endif]-->
<!--[if IE 7 ]>    <html class="ie7 no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>> <![endif]-->
<!--[if IE 8 ]>    <html class="ie8 no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>> <![endif]-->
<!--[if IE 9 ]>    <html class="ie9 no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>> <![endif]-->
<!--[if (gte IE 9)|(gt IEMobile 7)|!(IEMobile)|!(IE)]><!--><html class="no-js" lang="<?php echo $language->language ?>" dir="<?php echo $language->dir ?>"<?php echo $rdf_version . $rdf_namespaces ?>> <!--<![endif]-->
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