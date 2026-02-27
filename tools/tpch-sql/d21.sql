-- -- using default substitutions


-- select
-- 	s_name,
-- 	count(*) as numwait
-- from
-- 	supplier,
-- 	lineitem l1,
-- 	orders,
-- 	nation
-- where
-- 	s_suppkey = l1.l_suppkey
-- 	and o_orderkey = l1.l_orderkey
-- 	and o_orderstatus = 'F'
-- 	and l1.l_receiptdate > l1.l_commitdate
-- 	and exists (
-- 		select
-- 			*
-- 		from
-- 			lineitem l2
-- 		where
-- 			l2.l_orderkey = l1.l_orderkey
-- 			and l2.l_suppkey <> l1.l_suppkey
-- 	)
-- 	and not exists (
-- 		select
-- 			*
-- 		from
-- 			lineitem l3
-- 		where
-- 			l3.l_orderkey = l1.l_orderkey
-- 			and l3.l_suppkey <> l1.l_suppkey
-- 			and l3.l_receiptdate > l3.l_commitdate
-- 	)
-- 	and s_nationkey = n_nationkey
-- 	and n_name = 'SAUDI ARABIA'
-- group by
-- 	s_name
-- order by
-- 	numwait desc,
-- 	s_name
-- LIMIT 100;

WITH delayed AS (
    SELECT
        l_orderkey,
        l_suppkey,
        COUNT(*) FILTER (
            WHERE l_receiptdate > l_commitdate
        ) AS delay_cnt
    FROM lineitem
    GROUP BY l_orderkey, l_suppkey
),
order_stats AS (
    SELECT
        l_orderkey,
        COUNT(DISTINCT l_suppkey) AS supp_cnt,
        COUNT(*) FILTER (WHERE l_receiptdate > l_commitdate) AS total_delay_cnt
    FROM lineitem
    GROUP BY l_orderkey
)
SELECT
    s.s_name,
    COUNT(*) AS numwait
FROM delayed d
JOIN order_stats o
    ON o.l_orderkey = d.l_orderkey
JOIN orders ord
    ON ord.o_orderkey = d.l_orderkey
JOIN supplier s
    ON s.s_suppkey = d.l_suppkey
JOIN nation n
    ON n.n_nationkey = s.s_nationkey
WHERE
    d.delay_cnt > 0              -- 该 supplier 延迟
    AND o.supp_cnt > 1           -- 订单有多个 supplier
    AND o.total_delay_cnt = d.delay_cnt  -- 只有这一个 supplier 延迟
    AND ord.o_orderstatus = 'F'
    AND n.n_name = 'SAUDI ARABIA'
GROUP BY s.s_name
ORDER BY numwait DESC, s.s_name
LIMIT 100;