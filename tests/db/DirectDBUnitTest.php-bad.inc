<?php

// This is not a real plugin. Do not try to run this code.
// This merely contains intentionally INSECURE and UNSAFE examples of php code for testing.

// TODO: turn this into unit tests

return false; // Seriously, this should never be run.

function insecure_wpdb_query_1( $foo ) {
	global $wpdb;

	// 1. Unescaped query, string concat
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $foo . "' LIMIT 1" ); // unsafe
}


function insecure_wpdb_query_2( $foo ) {
	global $wpdb;

	// 2. Unescaped query, interpolated string
	$wpdb->query( "SELECT * FROM $wpdb->posts WHERE foo = '$foo' LIMIT 1" ); // unsafe
}


function insecure_wpdb_query_3( $foo ) {
	global $wpdb;

	// 3. Unescaped query, interpolated with {}
	$wpdb->query( "SELECT * FROM $wpdb->posts WHERE foo = '{$foo}' LIMIT 1" ); // unsafe
}

	
function insecure_wpdb_query_4( $foo ) {
	global $wpdb;

	// 4. Unescaped query, interpolated array
	$wpdb->query( "SELECT * FROM $wpdb->posts WHERE foo = '$foo[1]' LIMIT 1" ); // unsafe
}

function insecure_wpdb_query_5( $foo ) {
	global $wpdb;

	// 5. Unescaped query, superglobal
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $_POST['foo'] . "' LIMIT 1" ); // unsafe
}

function insecure_wpdb_query_6( $foo ) {
	global $wpdb;

	// 6. Unescaped object property, concat
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $foo->bar . "' LIMIT '" ); // unsafe
}

function insecure_wpdb_query_7( $foo ) {
	global $wpdb;

	// 7. Unescaped query, concat unknown function
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . baz( $foo ) . "' LIMIT '" ); // unsafe
}

function insecure_wpdb_query_8( $foo ) {
	global $wpdb;

	// 7. Unescaped query, $this->wpdb
	$this->wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $foo . "' LIMIT '" ); // unsafe
}

function insecure_wpdb_query_9( $foo ) {

	global $wpdb;

	// 9. Complex variable that is only partly escaped
	$esc[1]->foo = esc_sql( $foo );
	$wpdb->query( "SELECT * FROM $wpdb->posts WHERE foo = '{$esc[2]->foo}' LIMIT 1" ); // unsafe
}

function insecure_wpdb_query_10( $foo ) {
	global $wpdb;

	// 7. Unescaped query, prefixed with a constant
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . SOME_CONSTANT . $foo . "' LIMIT '" ); // unsafe
}

function insecure_wpdb_query_11( $foo ) {
	global $wpdb;

	$table = $wpdb->base_prefix . $foo;
	$wpdb->query( "SELECT * FROM $table WHERE foo=1 LIMIT 1" ); // unsafe because of $foo
}

function insecure_wpdb_query_12() {
	global $wpdb;

	// This looks safe at first glance
	$foo = ( isset( $_GET['foo'] ) ? absint( $_GET['foo'] ) : $_POST['foo'] );
	$wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $foo . "' LIMIT 1" ); // unsafe
}

function false_negative_1() {
	global $wpdb;

	// https://plugins.trac.wordpress.org/browser/automatic-domain-changer/trunk/auto-domain-change.php#L366
	// This wasn't caught for some reason
	$sql = 'UPDATE ' . $t . ' SET ' . implode( ',', $sets ) . ' WHERE `' . $id . '`=' . $row[ $id ] . ' LIMIT 1;';
	$wpdb->get_results($sql); // unsafe
}

function false_negative_2() {
	// https://plugins.trac.wordpress.org/browser/woo-wallet/trunk/includes/admin/class-woo-wallet-transaction-details.php#L100
	// This was caught in earlier revs but recently regressed
	$user_id = filter_input(INPUT_GET, 'user_id' );
	$this->total_count = (int) $wpdb->get_var( "SELECT COUNT(*) FROM {$wpdb->base_prefix}woo_wallet_transactions WHERE user_id={$user_id}" );
}

function insecure_wpdb_query_13( $foo ) {
	global $wpdb;

	$foo = not_an_escaping_function( $foo );
	$bar = $wpdb->query( "SELECT * FROM $wpdb->users WHERE foo = '" . $foo . "' LIMIT 1" ); // unsafe
}

function insecure_wpdb_query_14() {
	// Cribbed from some real code
	global $wpdb;

	parse_str( $_REQUEST['order'], $data );

	if ( is_array( $data ) ) {
			$id_arr = array( );
			foreach ( $data as $key => $values ) {
					foreach ( $values as $position => $id ) {
							$id_arr[] = $id;
					}
			}


			$menu_order_arr = array( );
			foreach ( $id_arr as $key => $id ) {
					$results = $wpdb->get_results( "SELECT menu_order FROM $wpdb->posts WHERE ID = " . $id ); // unsafe
			}

	}
}

function insecure_wpdb_query_14() {
	global $wpdb;

	$wpdb->get_results( "SELECT * FROM {$this->getPrefix()}_table WHERE 1=1 LIMIT 1" ); // unsafe but a warning rather than an error
}

function insecure_wpdb_query_15( $foo ) {
	global $wpdb;

	$bar = array(
		$foo
	);

	$wpdb->get_results( "SELECT * FROM $wpdb->posts WHERE ID IN ('" . implode( "', '", $bar ) . "') LIMIT 1" ); // unsafe
}

function insecure_wpdb_query_16( $foo ) {
	global $wpdb;

	$bar = $_POST['bad'];

	// $foo is correctly escaped here, but variables in the left hand side of a prepare() are unsafe!
	$wpdb->query( $wpdb->prepare( "SELECT * FROM $wpdb->users WHERE bar='$bar' AND foo=%s", $foo  ) ); // unsafe
}

function insecure_wpdb_query_17( $foo ) {
	global $wpdb;

	if ( $something ) {
		$bar = $foo;
	} else {
		$bar = esc_sql( $foo );
	}

	// This fails in some code paths, so it should trigger an error (but it's tricky to catch!)
	$wpdb->query( "SELECT * FROM $wpdb->posts WHERE ID = '$bar'" ); // unsafe
}

function insecure_wpdb_query_18( $where_clause ) {
	global $wpdb;

	// This should produce a warning instead of an error
	$wpdb->query( $wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND $where_clause", $foo ) ); // phpcs:disable WordPress.DB.PreparedSQL.InterpolatedNotPrepared

	// Old school warning
	$wpdb->query( $wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND $where_clause", $foo ) ); // phpcs:ignore cache ok, DB call ok, unprepared SQL ok.

	// Also this

	// phpcs:disable WordPress.DB.PreparedSQL.NotPrepared
	$wpdb->query( $wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND $where_clause", $foo ) ); // unsafe but suppressed
	// phpcs:enable

	// Error this time
	$wpdb->query( $wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND $where_clause", $foo ) ); // unsafe

	// Make sure we handle line numbers correctly
	$wpdb->query( 
		$wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND $where_clause", $foo )
	); // phpcs:ignore DB call ok

	$db_records_count = (int) $wpdb->get_var(
		$coupon_subquery // phpcs:ignore WordPress.DB.PreparedSQL.NotPrepared
	);
}

function dummy( ) {
	// This should have no effect on other tests
	$dummy_bar = esc_sql( $foo );
}

function insecure_wpdb_query_19() {

	$sql = $wpdb->prepare( "SELECT * FROM $wpdb->posts WHERE foo = %s AND bar = '$dummy_bar'", $foo );

	$wpdb->query( $sql ); // unsafe
}

function insecure_wpdb_query_20() {
		// Example from WooCommerce deliberately modified to make it insecure.

		/** @var wpdb $wpdb */
		global $wpdb;
		$query = "SELECT a.action_id FROM {$wpdb->actionscheduler_actions} a";
		$args  = [];
		if ( ! empty( $params[ 'group' ] ) ) {
			$query  .= " INNER JOIN {$wpdb->actionscheduler_groups} g ON g.group_id=a.group_id AND g.slug=%s";
			$args[] = $params[ 'group' ];
		}
		$query  .= " WHERE a.hook=%s";
		$args[] = $hook;
		if ( ! is_null( $params[ 'args' ] ) ) {
			$query  .= " AND a.args=%s";
			$args[] = $this->get_args_for_query( $params[ 'args' ] );
		}

		$order = 'ASC';
		if ( ! empty( $params[ 'status' ] ) ) {
			$query  .= " AND a.status=%s";
			$args[] = $params[ 'status' ];

			if ( self::STATUS_PENDING == $params[ 'status' ] ) {
				$order = $_POST['insecure']; // Modified to be unsafe
			} else {
				$order = 'DESC'; // Find the most recent action that matches.
			}
		}

		$query .= " ORDER BY scheduled_date_gmt $order LIMIT 1";

		$query = $wpdb->prepare( $query, $args );

		$id = $wpdb->get_var( $query );
}

function insecure_wpdb_query_21() {
	// Test concatenation

	global $wpdb;

	$sql = "SELECT * FROM $wpdb->users WHERE 1=1"; // safe so far..
	$sql .= " AND display_name = '$foo'"; // but not here!
	$sql .= $wpdb->prepare( ' LIMIT %d, %d', $offset, $limit ); // also safe here

	$result = $wpdb->get_row( $sql ); // unsafe
}

function insecure_wpdb_query_22() {
	// Variable table names should generate warnings, not errors.

	global $wpdb;

	$wpdb->get_var( $wpdb->prepare( "SELECT * FROM {$this->my_table_name} WHERE foo=%d", $foo ) );
	$wpdb->get_var( $wpdb->prepare( "SELECT * FROM ${this->my_table_name} WHERE foo=%d", $foo ) );
	$wpdb->get_var( $wpdb->prepare( "SELECT * FROM {$this->get_table_name()} WHERE foo=%d", $foo ) );
	$wpdb->get_var( $wpdb->prepare( "SELECT * FROM {$this->my_table_name}"
		. " WHERE foo=%d", $foo ) );


}

// From readme.md
function insecure_do_not_use( $ids, $status ) {
	global $wpdb;
	$in = "'" . join( "','", array_map( 'sanitize_text_field', $ids) ) . "'";
	$sql = "SELECT * FROM $wpdb->posts WHERE ID IN ($in)";
	return $wpdb->get_results( $wpdb->prepare( $sql . " AND post_status = %s", $status ) );
}