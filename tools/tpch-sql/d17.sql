-- -- using default substitutions


-- select
-- 	sum(l_extendedprice) / 7.0 as avg_yearly
-- from
-- 	lineitem,
-- 	part
-- where
-- 	p_partkey = l_partkey
-- 	and p_brand = 'Brand#23'
-- 	and p_container = 'MED BOX'
-- 	and l_quantity < (
-- 		select
-- 			0.2 * avg(l_quantity)
-- 		from
-- 			lineitem
-- 		where
-- 			l_partkey = p_partkey
-- 	);

WITH part_avg_quantity AS (
    SELECT
        l_partkey,
        0.2 * AVG(l_quantity) AS avg_qty
    FROM lineitem
    GROUP BY l_partkey
)
SELECT
    SUM(l_extendedprice) / 7.0 AS avg_yearly
FROM lineitem l
JOIN part p
    ON p.p_partkey = l.l_partkey
JOIN part_avg_quantity pq
    ON pq.l_partkey = l.l_partkey
WHERE p.p_brand = 'Brand#23'
  AND p.p_container = 'MED BOX'
  AND l.l_quantity < pq.avg_qty;