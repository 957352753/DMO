-- -- using default substitutions


-- select
-- 	s_name,
-- 	s_address
-- from
-- 	supplier,
-- 	nation
-- where
-- 	s_suppkey in (
-- 		select
-- 			ps_suppkey
-- 		from
-- 			partsupp
-- 		where
-- 			ps_partkey in (
-- 				select
-- 					p_partkey
-- 				from
-- 					part
-- 				where
-- 					p_name like 'forest%'
-- 			)
-- 			and ps_availqty > (
-- 				select
-- 					0.5 * sum(l_quantity)
-- 				from
-- 					lineitem
-- 				where
-- 					l_partkey = ps_partkey
-- 					and l_suppkey = ps_suppkey
-- 					and l_shipdate >= date '1994-01-01'
-- 					and l_shipdate < date '1994-01-01' + interval '1' year
-- 			)
-- 	)
-- 	and s_nationkey = n_nationkey
-- 	and n_name = 'CANADA'
-- order by
-- 	s_name;
WITH part_forest AS (
    SELECT p_partkey
    FROM part
    WHERE p_name LIKE 'forest%'
),
lineitem_1994 AS (
    SELECT
        l_partkey,
        l_suppkey,
        SUM(l_quantity) * 0.5 AS qty_limit
    FROM lineitem
    WHERE l_shipdate >= DATE '1994-01-01'
      AND l_shipdate <  DATE '1995-01-01'
    GROUP BY l_partkey, l_suppkey
)
SELECT
    s.s_name,
    s.s_address
FROM supplier s
JOIN nation n
    ON s.s_nationkey = n.n_nationkey
JOIN partsupp ps
    ON ps.ps_suppkey = s.s_suppkey
JOIN part_forest pf
    ON pf.p_partkey = ps.ps_partkey
JOIN lineitem_1994 l
    ON l.l_partkey = ps.ps_partkey
   AND l.l_suppkey = ps.ps_suppkey
WHERE n.n_name = 'CANADA'
  AND ps.ps_availqty > l.qty_limit
ORDER BY s.s_name;