<?php

if (drupal_is_front_page()) {
	$vars['front_pager'] = FALSE;
	unset($vars['title']);

	if (!empty($_GET['page'])) {
		$vars['front_pager'] = TRUE; 
	}
}

if (!empty($vars['node']) && $vars['node']->type !== 'page') { // A node which is not a page is displayed
  unset($vars['title']); // Unset the title – we set this in the node tpl
}
