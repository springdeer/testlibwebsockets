/*
 * libwebsockets - fulltext search
 *
 * Copyright (C) 2010-2018 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 *
 * included from libwebsockets.h
 */

/** \defgroup search Search
 *
 * ##Full-text search
 *
 * Lws provides superfast indexing and fulltext searching from index files on
 * storage.
 */
///@{

struct lws_fts;
struct lws_fts_file;

/*
 * Queries produce their results in an lwsac, using these public API types.
 * The first thing in the lwsac is always a struct lws_fts_result (see below)
 * containing heads for linked-lists of the other result types.
 */

/* one filepath's results */

struct lws_fts_result_filepath {
	struct lws_fts_result_filepath *next;
	int matches;	/* logical number of matches */
	int matches_length;	/* bytes in length table (may be zero) */
	int lines_in_file;
	int filepath_length;

	/* - uint32_t line table follows (first for alignment) */
	/* - filepath (of filepath_length) follows */
};

/* autocomplete result */

struct lws_fts_result_autocomplete {
	struct lws_fts_result_autocomplete *next;
	int instances;
	int agg_instances;
	int ac_length;
	char elided; /* children skipped in interest of antecedent children */
	char has_children;

	/* - autocomplete suggestion (of length ac_length) follows */
};

/*
 * The results lwsac always starts with this.  If no results and / or no
 * autocomplete the members may be NULL.  This implies the symbol nor any
 * suffix on it exists in the trie file.
 */
struct lws_fts_result {
	struct lws_fts_result_filepath *filepath_head;
	struct lws_fts_result_autocomplete *autocomplete_head;
	int duration_ms;
	int effective_flags; /* the search flags that were used */
};

LWS_VISIBLE LWS_EXTERN struct lws_fts *
lws_fts_create(int fd);

LWS_VISIBLE LWS_EXTERN void
lws_fts_destroy(struct lws_fts **trie);

LWS_VISIBLE LWS_EXTERN int
lws_fts_file_index(struct lws_fts *t, const char *filepath, int filepath_len,
		   int priority);

LWS_VISIBLE LWS_EXTERN int
lws_fts_fill(struct lws_fts *t, uint32_t file_index, const char *buf,
	     size_t len);

LWS_VISIBLE LWS_EXTERN int
lws_fts_serialize(struct lws_fts *t);

LWS_VISIBLE LWS_EXTERN int
lws_fts_adopt(struct lws_fts_file *jtf);

LWS_VISIBLE LWS_EXTERN struct lws_fts_file *
lws_fts_open(const char *filepath);

#define LWSFTS_F_QUERY_AUTOCOMPLETE	(1 << 0)
#define LWSFTS_F_QUERY_FILES		(1 << 1)
#define LWSFTS_F_QUERY_FILE_LINES	(1 << 2)
#define LWSFTS_F_QUERY_QUOTE_LINE	(1 << 3)

struct lws_fts_search_params {
	const char *needle;
	 /* if non-NULL, FILE results for this filepath only */
	const char *only_filepath;
	struct lwsac *results_head;
	int flags;
	int max_autocomplete;
	int max_files;
	int max_lines;
};

LWS_VISIBLE LWS_EXTERN struct lws_fts_result *
lws_fts_search(struct lws_fts_file *jtf, struct lws_fts_search_params *ftsp);

LWS_VISIBLE LWS_EXTERN void
lws_fts_close(struct lws_fts_file *jtf);

LWS_VISIBLE LWS_EXTERN int
lws_fts_results_dump(struct lws_fts_result *result);

///@}
