/**
 * Project: dQuote Selection
 * Version: 2.6.2
 * File: dquote.js
 * Author: digger @ http://mysmf.ru
 * License: The MIT License (MIT)
 */

var dQuoteText;


if (typeof oQuickReply != 'undefined') {
    oQuickReply.quote = insertReplyText;
}

if (typeof oEditorHandle_message != 'undefined') {
    insertQuoteFast = insertReplyText;
}

function getSelectedText() {

    if (window.getSelection) {
        return window.getSelection().toString();
    }
    else if (document.getSelection) {
        return document.getSelection();
    }
    else if (document.selection) {
        return document.selection.createRange().text;
    }
    return false;
}

function onTextReceived(XMLDoc) {

    var sQuoteText = '';
    for (var i = 0; i < XMLDoc.getElementsByTagName('quote')[0].childNodes.length; i++)
        sQuoteText += XMLDoc.getElementsByTagName('quote')[0].childNodes[i].nodeValue;

    if (typeof oEditorHandle_message != 'undefined' && oEditorHandle_message.bRichTextEnabled) {
        if (dQuoteText) oEditorHandle_message.insertText(sQuoteText.match(/^\[quote(.*)]/ig) + dQuoteText + '[/quote]' + '<br />', false, true)
        else oEditorHandle_message.insertText(sQuoteText + '<br />', false, true);
        oEditorHandle_message.focus();
    }
    else {
        if (dQuoteText) document.forms.postmodify.message.value += sQuoteText.match(/^\[quote(.*)]/ig) + dQuoteText + '[/quote]' + '\n'
        else document.forms.postmodify.message.value += sQuoteText + '\n';
        document.forms.postmodify.message.focus();
    }

    dQuoteText = '';
    ajax_indicator(false);
}

function insertReplyText(msgId) {

    dQuoteText = getSelectedText();
    if (dQuoteText == '' && typeof oQuickReply != 'undefined' && oQuickReply.bCollapsed) return true;

    ajax_indicator(true);
    getXMLDocument(smf_prepareScriptUrl(smf_scripturl) + 'action=quotefast;quote=' + msgId + ';xml;pb=message', onTextReceived);
    if (typeof oQuickReply != 'undefined' && oQuickReply.bCollapsed) oQuickReply.swap();

    window.location.hash = '#quickreply';

    return false;
}


