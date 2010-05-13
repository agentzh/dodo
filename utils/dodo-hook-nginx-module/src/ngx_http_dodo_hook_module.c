#include "ngx_http_dodo_hook_directive.h"
#include "ngx_http_dodo_hook_conf.h"

static ngx_command_t ngx_http_dodo_hook_cmds[] = {
	/* dodo_hook_enable [on | off] */
	{
		ngx_string("dodo_hook_enable"),
		NGX_HTTP_MAIN_CONF | NGX_CONF_TAKE1,
		ngx_conf_set_flag_slot,
		NGX_HTTP_MAIN_CONF_OFFSET,
		offsetof(ngx_http_dodo_hook_main_conf_t, enable_hook),
		NULL
	},

	/* dodo_hook_plugin /path/to/plugin/so/file */
	{
		ngx_string("dodo_hook_plugin"),
		NGX_HTTP_MAIN_CONF | NGX_CONF_TAKE1,
		ngx_http_dodo_hook_register_plugin,
		NGX_HTTP_MAIN_CONF_OFFSET,
		0,
		NULL
	},

	ngx_null_command
};

ngx_http_module_t ngx_http_dodo_hook_module_ctx = {
	NULL,							// preconfiguration
	ngx_http_dodo_hook_init_plugin,	// postconfiguration

	ngx_http_dodo_hook_create_main_conf,	// create main configuratiion
	ngx_http_dodo_hook_init_main_conf,	// init main configuration

	NULL,	// create server configuration
	NULL,	// merge server configuration

	NULL,	// create location configuration
	NULL	// merge location configuration
};

ngx_module_t ngx_http_dodo_hook_module = {
	NGX_MODULE_V1,
	&ngx_http_dodo_hook_module_ctx,	// module context
	ngx_http_dodo_hook_cmds,			// module directives
	NGX_HTTP_MODULE,				// module type
	NULL,							// init master
	NULL,							// init module
	NULL,							// init process
	NULL,							// init thread
	NULL,							// exit thread
	NULL,							// exit process
	NULL,							// exit master
	NGX_MODULE_V1_PADDING

};

