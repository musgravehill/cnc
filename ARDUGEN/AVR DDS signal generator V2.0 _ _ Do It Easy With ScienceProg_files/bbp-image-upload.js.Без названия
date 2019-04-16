/**
 * Author: Potent Plugins
 * License: GNU General Public License version 2 or later
 * License URI: http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
 */
function hm_bbpui_file_upload(field_id) {
	jQuery('#hm_bbpui_file_form, #hm_bbpui_file_frame').remove();
	jQuery('#' + field_id).parent().css('overflow', 'hidden')
		.append('<br /><form id="hm_bbpui_file_form" action="?hm_bbpui_do_upload=1" method="post" target="hm_bbpui_file_frame" enctype="multipart/form-data" onsubmit="hm_bbpui_file_upload_submit();" data-field-id="' + field_id + '">' +
		'<input type="file" name="hm_bbpui_file" id="hm_bbpui_file" tabindex="-1" onchange="jQuery(this).parent().submit();" />' +
		'<iframe id="hm_bbpui_file_frame" name="hm_bbpui_file_frame" />');
	jQuery('#hm_bbpui_file').trigger('click');
}

function hm_bbpui_file_upload_submit() {
	jQuery('#hm_bbpui_file_frame').load(function() {
		var response = jQuery('#hm_bbpui_file_frame').contents().find('body').text();
		if (response == '' || response == 'Error') {
			alert('The image could not be uploaded. Please check that it is a valid JPEG, PNG, or GIF file and that the file size is not too large.');
			response = '';
		}
		jQuery('#' + jQuery('#hm_bbpui_file_form').data('field-id')).val(response).prop('disabled', false);
		jQuery('#hm_bbpui_file_frame').off('load');
	});
	jQuery('#' + jQuery('#hm_bbpui_file_form').data('field-id')).prop('disabled', true).val('Please wait...');
	
	return true;
}

// Override addReply.moveForm function to re-initialize TinyMCE
var moveFormDefault, cancelReplyDefault;
jQuery(document).ready(function() {
	if (typeof addReply != 'undefined' && typeof addReply.moveForm != 'undefined') {
		//moveFormDefault = addReply.moveForm;
		
		
		
		/* This code copied from bbPress - https://bbpress.org/ - GNU GPL v2 */
		/* Copied to fix bug in moveForm function */
		moveFormDefault = function(replyId, parentId, respondId, postId) {
			var t = this, div, reply = t.I(replyId), respond = t.I(respondId), cancel = t.I('bbp-cancel-reply-to-link'), parent = t.I('bbp_reply_to'), post = t.I('bbp_topic_id');

			if ( ! reply || ! respond || ! cancel || ! parent )
				return;

			t.respondId = respondId;
			postId = postId || false;

			if ( ! t.I('bbp-temp-form-div') ) {
				div = document.createElement('div');
				div.id = 'bbp-temp-form-div';
				div.style.display = 'none';
				respond.parentNode.insertBefore(div, respond);
			}

			reply.parentNode.insertBefore(respond, null); // Bugfix - provide second argument
			if ( post && postId )
				post.value = postId;
			parent.value = parentId;
			cancel.style.display = '';

			cancel.onclick = function() {
				var t = addReply, temp = t.I('bbp-temp-form-div'), respond = t.I(t.respondId);

				if ( ! temp || ! respond )
					return;

				t.I('bbp_reply_to').value = '0';
				temp.parentNode.insertBefore(respond, temp);
				temp.parentNode.removeChild(temp);
				this.style.display = 'none';
				this.onclick = null;
				return false;
			}

			try { t.I('bbp_reply_content').focus(); }
			catch(e) {}

			return false;
		}
		/* End code copied from bbPress */
		
		
		
		addReply.moveForm = function(replyId, parentId, respondId, postId) {
			var tA = jQuery('#' + respondId).find('textarea.wp-editor-area');
			if (tA.length && typeof tinymce != 'undefined' && typeof tinyMCEPreInit != 'undefined'
					&& typeof tinyMCEPreInit.mceInit[tA.attr('id')] != undefined) {
				tinymce.get(tA.attr('id')).destroy();
				var result = moveFormDefault.apply(this, arguments);
				tinymce.init(tinyMCEPreInit.mceInit[tA.attr('id')]);
				
				
				jQuery('#bbp-cancel-reply-to-link').click(function() {
					var tA = jQuery(this).closest('form').find('textarea.wp-editor-area');
					if (tA.length && typeof tinymce != 'undefined' && typeof tinyMCEPreInit != 'undefined'
							&& typeof tinyMCEPreInit.mceInit[tA.attr('id')] != undefined) {
						tinymce.get(tA.attr('id')).destroy();
						tinymce.init(tinyMCEPreInit.mceInit[tA.attr('id')]);
					}
				});
				
				return result;
			} else {
				return moveFormDefault.apply(this, arguments);
			}
		}
		
		
	}
});