-- -- using default substitutions


-- select
-- 	s_acctbal,
-- 	s_name,
-- 	n_name,
-- 	p_partkey,
-- 	p_mfgr,
-- 	s_address,
-- 	s_phone,
-- 	s_comment
-- from
-- 	part,
-- 	supplier,
-- 	partsupp,
-- 	nation,
-- 	region
-- where
-- 	p_partkey = ps_partkey
-- 	and s_suppkey = ps_suppkey
-- 	and p_size = 15
-- 	and p_type like '%BRASS'
-- 	and s_nationkey = n_nationkey
-- 	and n_regionkey = r_regionkey
-- 	and r_name = 'EUROPE'
-- 	and ps_supplycost = (
-- 		select
-- 			min(ps_supplycost)
-- 		from
-- 			partsupp,
-- 			supplier,
-- 			nation,
-- 			region
-- 		where
-- 			p_partkey = ps_partkey
-- 			and s_suppkey = ps_suppkey
-- 			and s_nationkey = n_nationkey
-- 			and n_regionkey = r_regionkey
-- 			and r_name = 'EUROPE'
-- 	)
-- order by
-- 	s_acctbal desc,
-- 	n_name,
-- 	s_name,
-- 	p_partkey;
-- LIMIT 100

WITH europe_min_cost AS (
    SELECT
        ps.ps_partkey,
        MIN(ps.ps_supplycost) AS min_supplycost
    FROM partsupp ps
    JOIN supplier s
        ON s.s_suppkey = ps.ps_suppkey
    JOIN nation n
        ON n.n_nationkey = s.s_nationkey
    JOIN region r
        ON r.r_regionkey = n.n_regionkey
    WHERE r.r_name = 'EUROPE'
    GROUP BY ps.ps_partkey
)
SELECT
    s.s_acctbal,
    s.s_name,
    n.n_name,
    p.p_partkey,
    p.p_mfgr,
    s.s_address,
    s.s_phone,
    s.s_comment
FROM part p
JOIN partsupp ps
    ON p.p_partkey = ps.ps_partkey
JOIN supplier s
    ON s.s_suppkey = ps.ps_suppkey
JOIN nation n
    ON n.n_nationkey = s.s_nationkey
JOIN region r
    ON r.r_regionkey = n.n_regionkey
JOIN europe_min_cost em
    ON em.ps_partkey = ps.ps_partkey
   AND em.min_supplycost = ps.ps_supplycost
WHERE p.p_size = 15
  AND p.p_type LIKE '%BRASS'
  AND r.r_name = 'EUROPE'
ORDER BY
    s.s_acctbal DESC,
    n.n_name,
    s.s_name,
    p.p_partkey
LIMIT 100;