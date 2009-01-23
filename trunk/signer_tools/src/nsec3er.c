/**
 * This tool creates NSEC3 records
 *
 * This code is provided AS-IS, you know the drill, use at own risk
 * 
 * Written by Jelte Jansen
 * 
 * Copyright 2008 NLnet Labs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <ldns/ldns.h>

bool
is_same_name(ldns_rr *a, ldns_rr *b)
{
	if (!a || !b) {
		return false;
	} else if (ldns_dname_compare(ldns_rr_owner(a),
	                              ldns_rr_owner(b)) != 0) {
		return false;
	} else {
		return true;
	}
}

void
usage(FILE *out)
{
	fprintf(out, "Usage: nseccer [options]\n");
	fprintf(out, "Options:\n");
	fprintf(out, "-e\t\tDon't echo input\n");
	fprintf(out, "-f <file>\tRead RR's from file instead of stdin\n");
	fprintf(out, "-h\t\tShow this text\n");
	fprintf(out, "-n\t\tDon't echo the input records\n");
	//fprintf(out, "-o\t(mandatory) dname of the zone\n");
	fprintf(out, "-v <level>\tVerbosity level\n");
	fprintf(out, "\n");
	fprintf(out, "When a new owner name is read (or input stops),\n");
	fprintf(out, "an NSEC record is created from the previous to\n");
	fprintf(out, "the new owner name. All rr types seen with the\n");
	fprintf(out, "previous owner name are added to this new NSEC\n");
	fprintf(out, "Resource Record\n");
	fprintf(out, "These records are then printed to stdout\n");
}

int
main(int argc, char **argv)
{
	int verbosity = 5;
	int c;
	bool echo_input = true;
	FILE *input_file = stdin;

	ldns_rdf *origin = NULL;
	uint32_t ttl = LDNS_DEFAULT_TTL;
	
	ldns_rr *first_rr;
	ldns_rr *cur_rr;
	ldns_rr *prev_rr = NULL;
	ldns_rdf *prev_name = NULL;
	ldns_rr_list *cur_rrset;
	ldns_status status;
	
	/* creation values */
	ldns_rr *nsec_rr;
	uint32_t nsec_ttl = LDNS_DEFAULT_TTL;
	
	uint8_t algorithm = 1;
	uint8_t flags = 0;
	uint16_t iterations = 1;
	uint8_t salt_length = 0;
	uint8_t *salt = NULL;
	ldns_rdf *next_hash;

	while ((c = getopt(argc, argv, "ef:o:v:")) != -1) {
		switch(c) {
			case 'e':
				echo_input = false;
				break;
			case 'f':
				input_file = fopen(optarg, "r");
				if (!input_file) {
					fprintf(stderr,
					        "Error opening %s: %s\n",
					        optarg,
					        strerror(errno));
					break;
				}
			case 'h':
				usage(stderr);
				exit(0);
				break;
			case 'o':
				status = ldns_str2rdf_dname(&origin, optarg);
				if (!origin) {
					fprintf(stderr,
					        "Error parsing origin '%s': %s\n",
					        optarg,
					        ldns_get_errorstr_by_id(status));
					exit(1);
				}
				break;
			case 'v':
				verbosity = atoi(optarg);
				break;
			default:
				usage(stderr);
				exit(1);
				break;
		}
	}
	
	/* origin is mandatory for creating the correct nsec3 owner names
	 */
	if (!origin) {
		fprintf(stderr, "Error: no origin given to nsec3er (-o)\n");
		exit(2);
	}


	/* Parse mode: read rrs from input */
	/* Directives like $ORIGIN and $TTL are not allowed */
	status = ldns_rr_new_frm_fp(&cur_rr,
	                            input_file,
	                            &ttl,
	                            &origin,
	                            &prev_name);
	
	if (status != LDNS_STATUS_OK) {
		fprintf(stderr,
		        "Error in first RR: %s, aborting\n",
		        ldns_get_errorstr_by_id(status));
		exit(1);
	}
	
	cur_rrset = ldns_rr_list_new();
	prev_rr = cur_rr;
	first_rr = ldns_rr_clone(cur_rr);

	while (status == LDNS_STATUS_OK) {
		if (is_same_name(prev_rr, cur_rr)) {
			if (ldns_rr_get_type(cur_rr) == LDNS_RR_TYPE_SOA) {
				nsec_ttl = ldns_rdf2native_int32(ldns_rr_rdf(cur_rr,
				                                             6));
			}
			ldns_rr_list_push_rr(cur_rrset, cur_rr);
		} else {
			/* handle rrset */
			if (echo_input) {
				ldns_rr_list_print(stdout, cur_rrset);
			}
			
			/* create nsec and print it */
			nsec_rr = ldns_create_nsec3(ldns_rr_owner(prev_rr),
			                            origin,
			                            cur_rrset,
			                            algorithm,
			                            flags,
			                            iterations,
			                            salt_length,
			                            salt,
			                            false);
			ldns_rr_set_ttl(nsec_rr, nsec_ttl);
			/* todo; we are hashing names twice at the moment */
			next_hash = ldns_nsec3_hash_name(ldns_rr_owner(cur_rr),
			                                 algorithm,
			                                 iterations,
			                                 salt_length,
			                                 salt);

			ldns_rr_set_rdf(nsec_rr, 
			                ldns_dname_label(next_hash, 0),
			                4);

			ldns_rdf_deep_free(next_hash);
			ldns_rr_print(stdout, nsec_rr);

			ldns_rr_free(nsec_rr);
			
			/* clean for next set */
			ldns_rr_list_deep_free(cur_rrset);
			cur_rrset = ldns_rr_list_new();
			ldns_rr_list_push_rr(cur_rrset, cur_rr);
		}

		prev_rr = cur_rr;
		cur_rr = NULL;
		status = ldns_rr_new_frm_fp(&cur_rr,
		                            input_file,
		                            &ttl,
		                            &origin,
		                            &prev_name);
	}

	if (status != LDNS_STATUS_OK &&
	    status != LDNS_STATUS_SYNTAX_EMPTY) {
		if (verbosity >= 1) {
			fprintf(stderr,
			        "Error parsing RR: %s, aborting\n",
			        ldns_get_errorstr_by_id(status));
			fprintf(stderr,
			        "Last RR sucessfully read: ");
			ldns_rr_print(stderr, prev_rr);
			exit(2);
		}
	}
	/* create final nsec and print it */
	if (echo_input) {
		ldns_rr_list_print(stdout, cur_rrset);
	}
	nsec_rr = ldns_create_nsec3(ldns_rr_owner(prev_rr),
								origin,
								cur_rrset,
								algorithm,
								flags,
								iterations,
								salt_length,
								salt,
								false);
	ldns_rr_set_ttl(nsec_rr, nsec_ttl);
	/* todo; we are hashing names twice at the moment */
	ldns_rr_set_rdf(nsec_rr, 
					ldns_nsec3_hash_name(origin,
										 algorithm,
										 iterations,
										 salt_length,
										 salt),
					4);
	ldns_rr_print(stdout, nsec_rr);
	ldns_rr_free(nsec_rr);
	ldns_rr_free(first_rr);
	ldns_rr_list_deep_free(cur_rrset);
	if (prev_name) {
		ldns_rdf_deep_free(prev_name);
	}
	
	if (input_file != stdin) {
		fclose(input_file);
	}
	
	return 0;
}
