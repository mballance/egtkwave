#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: ./verilog.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1

#ifdef __USE_PROTOS
void v_source_text(void);
#else
extern void v_source_text();
#endif

#ifdef __USE_PROTOS
void v_description(void);
#else
extern void v_description();
#endif

#ifdef __USE_PROTOS
void v_config(void);
#else
extern void v_config();
#endif

#ifdef __USE_PROTOS
void v_module_parameters(void);
#else
extern void v_module_parameters();
#endif

#ifdef __USE_PROTOS
void v_module(void);
#else
extern void v_module();
#endif

#ifdef __USE_PROTOS
void v_module_body(void);
#else
extern void v_module_body();
#endif

#ifdef __USE_PROTOS
void v_v2k_list_of_ports(void);
#else
extern void v_v2k_list_of_ports();
#endif

#ifdef __USE_PROTOS
void v_v2k_iio_declaration(void);
#else
extern void v_v2k_iio_declaration();
#endif

#ifdef __USE_PROTOS
void v_v2k_input_declaration(void);
#else
extern void v_v2k_input_declaration();
#endif

#ifdef __USE_PROTOS
void v_v2k_output_declaration(void);
#else
extern void v_v2k_output_declaration();
#endif

#ifdef __USE_PROTOS
void v_v2k_inout_declaration(void);
#else
extern void v_v2k_inout_declaration();
#endif

#ifdef __USE_PROTOS
void v_list_of_ports(void);
#else
extern void v_list_of_ports();
#endif

#ifdef __USE_PROTOS
void v_port(void);
#else
extern void v_port();
#endif

#ifdef __USE_PROTOS
void v_port_expression(void);
#else
extern void v_port_expression();
#endif

#ifdef __USE_PROTOS
void v_port_reference(void);
#else
extern void v_port_reference();
#endif

#ifdef __USE_PROTOS
void v_module_item(void);
#else
extern void v_module_item();
#endif

#ifdef __USE_PROTOS
void v_udp(void);
#else
extern void v_udp();
#endif

#ifdef __USE_PROTOS
void v_udp_declaration(void);
#else
extern void v_udp_declaration();
#endif

#ifdef __USE_PROTOS
void v_udp_initial_statement(void);
#else
extern void v_udp_initial_statement();
#endif

#ifdef __USE_PROTOS
void v_init_val(void);
#else
extern void v_init_val();
#endif

#ifdef __USE_PROTOS
void v_output_terminal_name(void);
#else
extern void v_output_terminal_name();
#endif

#ifdef __USE_PROTOS
void v_table_definition(void);
#else
extern void v_table_definition();
#endif

#ifdef __USE_PROTOS
void v_table_entries(void);
#else
extern void v_table_entries();
#endif

#ifdef __USE_PROTOS
void v_com_seq_entry(void);
#else
extern void v_com_seq_entry();
#endif

#ifdef __USE_PROTOS
void v_level_symbol(void);
#else
extern void v_level_symbol();
#endif

#ifdef __USE_PROTOS
void v_edge(void);
#else
extern void v_edge();
#endif

#ifdef __USE_PROTOS
void v_fake_edge(void);
#else
extern void v_fake_edge();
#endif

#ifdef __USE_PROTOS
void v_level_symbol2(void);
#else
extern void v_level_symbol2();
#endif

#ifdef __USE_PROTOS
void v_input_list(void);
#else
extern void v_input_list();
#endif

#ifdef __USE_PROTOS
void v_state(void);
#else
extern void v_state();
#endif

#ifdef __USE_PROTOS
void v_next_state(void);
#else
extern void v_next_state();
#endif

#ifdef __USE_PROTOS
void v_task(void);
#else
extern void v_task();
#endif

#ifdef __USE_PROTOS
void v_function(void);
#else
extern void v_function();
#endif

#ifdef __USE_PROTOS
void v_range_or_type(void);
#else
extern void v_range_or_type();
#endif

#ifdef __USE_PROTOS
void v_tf_declaration(void);
#else
extern void v_tf_declaration();
#endif

#ifdef __USE_PROTOS
void v_parameter_declaration(void);
#else
extern void v_parameter_declaration();
#endif

#ifdef __USE_PROTOS
void v_localparam_declaration(void);
#else
extern void v_localparam_declaration();
#endif

#ifdef __USE_PROTOS
void v_param_assignment(void);
#else
extern void v_param_assignment();
#endif

#ifdef __USE_PROTOS
void v_list_of_param_assignments(void);
#else
extern void v_list_of_param_assignments();
#endif

#ifdef __USE_PROTOS
void v_input_declaration(void);
#else
extern void v_input_declaration();
#endif

#ifdef __USE_PROTOS
void v_output_declaration(void);
#else
extern void v_output_declaration();
#endif

#ifdef __USE_PROTOS
void v_inout_declaration(void);
#else
extern void v_inout_declaration();
#endif

#ifdef __USE_PROTOS
void v_net_chg(void);
#else
extern void v_net_chg();
#endif

#ifdef __USE_PROTOS
void v_nettype(void);
#else
extern void v_nettype();
#endif

#ifdef __USE_PROTOS
void v_optnettype(void);
#else
extern void v_optnettype();
#endif

#ifdef __USE_PROTOS
void v_expandrange(void);
#else
extern void v_expandrange();
#endif

#ifdef __USE_PROTOS
void v_reg_declaration(void);
#else
extern void v_reg_declaration();
#endif

#ifdef __USE_PROTOS
void v_reg_range(void);
#else
extern void v_reg_range();
#endif

#ifdef __USE_PROTOS
void v_time_declaration(void);
#else
extern void v_time_declaration();
#endif

#ifdef __USE_PROTOS
void v_integer_declaration(void);
#else
extern void v_integer_declaration();
#endif

#ifdef __USE_PROTOS
void v_genvar_declaration(void);
#else
extern void v_genvar_declaration();
#endif

#ifdef __USE_PROTOS
void v_real_declaration(void);
#else
extern void v_real_declaration();
#endif

#ifdef __USE_PROTOS
void v_event_declaration(void);
#else
extern void v_event_declaration();
#endif

#ifdef __USE_PROTOS
void v_continuous_assign(void);
#else
extern void v_continuous_assign();
#endif

#ifdef __USE_PROTOS
void v_cont_drv(void);
#else
extern void v_cont_drv();
#endif

#ifdef __USE_PROTOS
void v_cont_exr(void);
#else
extern void v_cont_exr();
#endif

#ifdef __USE_PROTOS
void v_cont_dly(void);
#else
extern void v_cont_dly();
#endif

#ifdef __USE_PROTOS
void v_parameter_override(void);
#else
extern void v_parameter_override();
#endif

#ifdef __USE_PROTOS
void v_list_of_variables(void);
#else
extern void v_list_of_variables();
#endif

#ifdef __USE_PROTOS
void v_optrange_list(void);
#else
extern void v_optrange_list();
#endif

#ifdef __USE_PROTOS
void v_name_of_variable(void);
#else
extern void v_name_of_variable();
#endif

#ifdef __USE_PROTOS
void v_list_of_register_variables(void);
#else
extern void v_list_of_register_variables();
#endif

#ifdef __USE_PROTOS
void v_register_variable(void);
#else
extern void v_register_variable();
#endif

#ifdef __USE_PROTOS
void v_name_of_register(void);
#else
extern void v_name_of_register();
#endif

#ifdef __USE_PROTOS
void v_name_of_memory(void);
#else
extern void v_name_of_memory();
#endif

#ifdef __USE_PROTOS
void v_name_of_event(void);
#else
extern void v_name_of_event();
#endif

#ifdef __USE_PROTOS
void v_charge_strength(void);
#else
extern void v_charge_strength();
#endif

#ifdef __USE_PROTOS
void v_drive_strength(void);
#else
extern void v_drive_strength();
#endif

#ifdef __USE_PROTOS
void v_strength0(void);
#else
extern void v_strength0();
#endif

#ifdef __USE_PROTOS
void v_strength1(void);
#else
extern void v_strength1();
#endif

#ifdef __USE_PROTOS
void v_optsigned(void);
#else
extern void v_optsigned();
#endif

#ifdef __USE_PROTOS
void v_range(void);
#else
extern void v_range();
#endif

#ifdef __USE_PROTOS
void v_list_of_assignments(void);
#else
extern void v_list_of_assignments();
#endif

#ifdef __USE_PROTOS
void v_gate_declaration(void);
#else
extern void v_gate_declaration();
#endif

#ifdef __USE_PROTOS
void v_gatetype(void);
#else
extern void v_gatetype();
#endif

#ifdef __USE_PROTOS
void v_gate_drv(void);
#else
extern void v_gate_drv();
#endif

#ifdef __USE_PROTOS
void v_gate_dly(void);
#else
extern void v_gate_dly();
#endif

#ifdef __USE_PROTOS
void v_gate_range(void);
#else
extern void v_gate_range();
#endif

#ifdef __USE_PROTOS
void v_gate_instance(void);
#else
extern void v_gate_instance();
#endif

#ifdef __USE_PROTOS
void v_name_of_gate_instance(void);
#else
extern void v_name_of_gate_instance();
#endif

#ifdef __USE_PROTOS
void v_terminal(void);
#else
extern void v_terminal();
#endif

#ifdef __USE_PROTOS
void v_udp_instantiation(void);
#else
extern void v_udp_instantiation();
#endif

#ifdef __USE_PROTOS
void v_name_of_udp(void);
#else
extern void v_name_of_udp();
#endif

#ifdef __USE_PROTOS
void v_udp_instance(void);
#else
extern void v_udp_instance();
#endif

#ifdef __USE_PROTOS
void v_name_of_udp_instance(void);
#else
extern void v_name_of_udp_instance();
#endif

#ifdef __USE_PROTOS
void v_module_instantiation(void);
#else
extern void v_module_instantiation();
#endif

#ifdef __USE_PROTOS
void v_name_of_module(void);
#else
extern void v_name_of_module();
#endif

#ifdef __USE_PROTOS
void v_parameter_value_assignment(void);
#else
extern void v_parameter_value_assignment();
#endif

#ifdef __USE_PROTOS
void v_module_instance(void);
#else
extern void v_module_instance();
#endif

#ifdef __USE_PROTOS
void v_name_of_instance_opt(void);
#else
extern void v_name_of_instance_opt();
#endif

#ifdef __USE_PROTOS
void v_name_of_instance(void);
#else
extern void v_name_of_instance();
#endif

#ifdef __USE_PROTOS
void v_mod_range(void);
#else
extern void v_mod_range();
#endif

#ifdef __USE_PROTOS
void v_list_of_module_connections(void);
#else
extern void v_list_of_module_connections();
#endif

#ifdef __USE_PROTOS
void v_module_port_connection(void);
#else
extern void v_module_port_connection();
#endif

#ifdef __USE_PROTOS
void v_named_port_connection(void);
#else
extern void v_named_port_connection();
#endif

#ifdef __USE_PROTOS
void v_initial_statement(void);
#else
extern void v_initial_statement();
#endif

#ifdef __USE_PROTOS
void v_always_statement(void);
#else
extern void v_always_statement();
#endif

#ifdef __USE_PROTOS
void v_statement_or_null(void);
#else
extern void v_statement_or_null();
#endif

#ifdef __USE_PROTOS
void v_statement(void);
#else
extern void v_statement();
#endif

#ifdef __USE_PROTOS
void v_assignment(void);
#else
extern void v_assignment();
#endif

#ifdef __USE_PROTOS
void v_block_or_non_assignment(void);
#else
extern void v_block_or_non_assignment();
#endif

#ifdef __USE_PROTOS
void v_blocking_assignment(void);
#else
extern void v_blocking_assignment();
#endif

#ifdef __USE_PROTOS
void v_non_blocking_assignment(void);
#else
extern void v_non_blocking_assignment();
#endif

#ifdef __USE_PROTOS
void v_delay_or_event_control(void);
#else
extern void v_delay_or_event_control();
#endif

#ifdef __USE_PROTOS
void v_delay_or_event_control_stmt(void);
#else
extern void v_delay_or_event_control_stmt();
#endif

#ifdef __USE_PROTOS
void v_case_item(void);
#else
extern void v_case_item();
#endif

#ifdef __USE_PROTOS
void v_seq_block(void);
#else
extern void v_seq_block();
#endif

#ifdef __USE_PROTOS
void v_par_block(void);
#else
extern void v_par_block();
#endif

#ifdef __USE_PROTOS
void v_name_of_block(void);
#else
extern void v_name_of_block();
#endif

#ifdef __USE_PROTOS
void v_block_declaration(void);
#else
extern void v_block_declaration();
#endif

#ifdef __USE_PROTOS
void v_task_enable(void);
#else
extern void v_task_enable();
#endif

#ifdef __USE_PROTOS
void v_name_of_task(void);
#else
extern void v_name_of_task();
#endif

#ifdef __USE_PROTOS
void v_name_of_task_or_block(void);
#else
extern void v_name_of_task_or_block();
#endif

#ifdef __USE_PROTOS
void v_generate_block(void);
#else
extern void v_generate_block();
#endif

#ifdef __USE_PROTOS
void v_specify_block(void);
#else
extern void v_specify_block();
#endif

#ifdef __USE_PROTOS
void v_constant_expression(void);
#else
extern void v_constant_expression();
#endif

#ifdef __USE_PROTOS
void v_lvalue(void);
#else
extern void v_lvalue();
#endif

#ifdef __USE_PROTOS
void v_expression(void);
#else
extern void v_expression();
#endif

#ifdef __USE_PROTOS
void v_expression2(void);
#else
extern void v_expression2();
#endif

#ifdef __USE_PROTOS
void v_mintypmax_expression(void);
#else
extern void v_mintypmax_expression();
#endif

#ifdef __USE_PROTOS
void v_unary_operator(void);
#else
extern void v_unary_operator();
#endif

#ifdef __USE_PROTOS
void v_binary_operator(void);
#else
extern void v_binary_operator();
#endif

#ifdef __USE_PROTOS
void v_opt_array_handling(void);
#else
extern void v_opt_array_handling();
#endif

#ifdef __USE_PROTOS
void v_primary(void);
#else
extern void v_primary();
#endif

#ifdef __USE_PROTOS
void v_number(void);
#else
extern void v_number();
#endif

#ifdef __USE_PROTOS
void v_concatenation(void);
#else
extern void v_concatenation();
#endif

#ifdef __USE_PROTOS
void v_multiple_concatenation(void);
#else
extern void v_multiple_concatenation();
#endif

#ifdef __USE_PROTOS
void v_function_call(void);
#else
extern void v_function_call();
#endif

#ifdef __USE_PROTOS
void v_name_of_function(void);
#else
extern void v_name_of_function();
#endif

#ifdef __USE_PROTOS
void v_explist(void);
#else
extern void v_explist();
#endif

#ifdef __USE_PROTOS
void v_fn_expression(void);
#else
extern void v_fn_expression();
#endif

#ifdef __USE_PROTOS
void v_mexplist(void);
#else
extern void v_mexplist();
#endif

#ifdef __USE_PROTOS
void v_mfn_expression(void);
#else
extern void v_mfn_expression();
#endif

#ifdef __USE_PROTOS
void v_identifier(void);
#else
extern void v_identifier();
#endif

#ifdef __USE_PROTOS
void v_identifier_nodot(void);
#else
extern void v_identifier_nodot();
#endif

#ifdef __USE_PROTOS
void v_delay(void);
#else
extern void v_delay();
#endif

#ifdef __USE_PROTOS
void v_delay_control(void);
#else
extern void v_delay_control();
#endif

#ifdef __USE_PROTOS
void v_event_control(void);
#else
extern void v_event_control();
#endif

#ifdef __USE_PROTOS
void v_event_expression(void);
#else
extern void v_event_expression();
#endif

#ifdef __USE_PROTOS
void v_orcomma(void);
#else
extern void v_orcomma();
#endif

#ifdef __USE_PROTOS
void v_event_expression2(void);
#else
extern void v_event_expression2();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType zzerr3[];
extern SetWordType zzerr4[];
extern SetWordType zzerr5[];
extern SetWordType setwd1[];
extern SetWordType zzerr6[];
extern SetWordType zzerr7[];
extern SetWordType zzerr8[];
extern SetWordType zzerr9[];
extern SetWordType zzerr10[];
extern SetWordType setwd2[];
extern SetWordType zzerr11[];
extern SetWordType zzerr12[];
extern SetWordType zzerr13[];
extern SetWordType zzerr14[];
extern SetWordType setwd3[];
extern SetWordType zzerr15[];
extern SetWordType zzerr16[];
extern SetWordType zzerr17[];
extern SetWordType zzerr18[];
extern SetWordType zzerr19[];
extern SetWordType setwd4[];
extern SetWordType zzerr20[];
extern SetWordType zzerr21[];
extern SetWordType zzerr22[];
extern SetWordType setwd5[];
extern SetWordType zzerr23[];
extern SetWordType zzerr24[];
extern SetWordType zzerr25[];
extern SetWordType zzerr26[];
extern SetWordType setwd6[];
extern SetWordType zzerr27[];
extern SetWordType zzerr28[];
extern SetWordType zzerr29[];
extern SetWordType setwd7[];
extern SetWordType zzerr30[];
extern SetWordType zzerr31[];
extern SetWordType setwd8[];
extern SetWordType zzerr32[];
extern SetWordType zzerr33[];
extern SetWordType zzerr34[];
extern SetWordType zzerr35[];
extern SetWordType setwd9[];
extern SetWordType zzerr36[];
extern SetWordType zzerr37[];
extern SetWordType zzerr38[];
extern SetWordType zzerr39[];
extern SetWordType setwd10[];
extern SetWordType zzerr40[];
extern SetWordType zzerr41[];
extern SetWordType zzerr42[];
extern SetWordType zzerr43[];
extern SetWordType setwd11[];
extern SetWordType zzerr44[];
extern SetWordType zzerr45[];
extern SetWordType zzerr46[];
extern SetWordType zzerr47[];
extern SetWordType setwd12[];
extern SetWordType zzerr48[];
extern SetWordType zzerr49[];
extern SetWordType zzerr50[];
extern SetWordType setwd13[];
extern SetWordType zzerr51[];
extern SetWordType zzerr52[];
extern SetWordType zzerr53[];
extern SetWordType setwd14[];
extern SetWordType zzerr54[];
extern SetWordType setwd15[];
extern SetWordType zzerr55[];
extern SetWordType zzerr56[];
extern SetWordType zzerr57[];
extern SetWordType setwd16[];
extern SetWordType zzerr58[];
extern SetWordType zzerr59[];
extern SetWordType zzerr60[];
extern SetWordType zzerr61[];
extern SetWordType setwd17[];
extern SetWordType zzerr62[];
extern SetWordType zzerr63[];
extern SetWordType zzerr64[];
extern SetWordType zzerr65[];
extern SetWordType setwd18[];
extern SetWordType zzerr66[];
extern SetWordType zzerr67[];
extern SetWordType zzerr68[];
extern SetWordType setwd19[];
extern SetWordType zzerr69[];
extern SetWordType setwd20[];
extern SetWordType zzerr70[];
extern SetWordType zzerr71[];
extern SetWordType zzerr72[];
extern SetWordType zzerr73[];
extern SetWordType zzerr74[];
extern SetWordType setwd21[];
extern SetWordType zzerr75[];
extern SetWordType zzerr76[];
extern SetWordType setwd22[];
extern SetWordType zzerr77[];
extern SetWordType zzerr78[];
extern SetWordType zzerr79[];
extern SetWordType setwd23[];
extern SetWordType zzerr80[];
extern SetWordType zzerr81[];
extern SetWordType zzerr82[];
extern SetWordType setwd24[];
extern SetWordType zzerr83[];
extern SetWordType zzerr84[];
extern SetWordType zzerr85[];
extern SetWordType zzerr86[];
extern SetWordType setwd25[];
extern SetWordType zzerr87[];
extern SetWordType zzerr88[];
extern SetWordType zzerr89[];
extern SetWordType setwd26[];
extern SetWordType zzerr90[];
extern SetWordType zzerr91[];
extern SetWordType setwd27[];
extern SetWordType zzerr92[];
extern SetWordType zzerr93[];
extern SetWordType zzerr94[];
extern SetWordType zzerr95[];
extern SetWordType zzerr96[];
extern SetWordType zzerr97[];
extern SetWordType setwd28[];
extern SetWordType zzerr98[];
extern SetWordType zzerr99[];
extern SetWordType zzerr100[];
extern SetWordType zzerr101[];
extern SetWordType setwd29[];
extern SetWordType zzerr102[];
extern SetWordType zzerr103[];
extern SetWordType zzerr104[];
extern SetWordType zzerr105[];
extern SetWordType zzerr106[];
extern SetWordType zzerr107[];
extern SetWordType setwd30[];
extern SetWordType zzerr108[];
extern SetWordType zzerr109[];
extern SetWordType zzerr110[];
extern SetWordType zzerr111[];
extern SetWordType zzerr112[];
extern SetWordType setwd31[];
extern SetWordType zzerr113[];
extern SetWordType zzerr114[];
extern SetWordType setwd32[];
extern SetWordType zzerr115[];
extern SetWordType zzerr116[];
extern SetWordType zzerr117[];
extern SetWordType setwd33[];
extern SetWordType zzerr118[];
extern SetWordType zzerr119[];
extern SetWordType zzerr120[];
extern SetWordType setwd34[];
extern SetWordType zzerr121[];
extern SetWordType zzerr122[];
extern SetWordType setwd35[];
