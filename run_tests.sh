#!/usr/bin/env bash
# ============================================================
# run_tests.sh  —  Member 3 unit tests
# Tests: Round Robin, Priority Scheduling, Metrics Calculation
# ============================================================

PASS=0; FAIL=0
RED='\033[0;31m'; GREEN='\033[0;32m'; NC='\033[0m'

# ── helpers ──────────────────────────────────────────────────

check() {
    local name="$1" actual="$2" expected="$3"
    if [ "$actual" = "$expected" ]; then
        echo -e "  ${GREEN}PASS${NC}  $name"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}FAIL${NC}  $name"
        echo    "        expected : '$expected'"
        echo    "        actual   : '$actual'"
        FAIL=$((FAIL+1))
    fi
}

# Extract the body of a named scheduler section.
# Stops at the next "===" header or end-of-output.
get_section() {
    local algo="$1" text="$2"
    echo "$text" | awk -v a="$algo" '
        /^=== / {
            if (in_sec) { in_sec=0 }
            if (index($0,a)) { in_sec=1 }
            next            # skip the header line itself
        }
        in_sec { print }
    '
}

# Get the value of a named column for a given PID from the process table.
# Column names: Arrival Burst Start Completion Waiting Turnaround Response
get_field() {
    local pid="$1" col="$2" text="$3"
    echo "$text" | awk -v p="$pid" -v c="$col" '
        /^PID/ {
            for (i=1;i<=NF;i++) hdr[i]=$i
            next
        }
        /^----/ { next }
        $1==p {
            for (i=1;i<=NF;i++) if (hdr[i]==c) { print $i; exit }
        }
    '
}

# Get a metric value by keyword (strips trailing % and units).
get_metric() {
    local key="$1" text="$2"
    echo "$text" | grep "$key" | awk -F': ' '{ val=$2; gsub(/%/,"",val); print val+0 }' \
        | awk '{printf "%.2f", $1}'
}

get_throughput() {
    local text="$1"
    echo "$text" | grep "Throughput" | awk -F': ' '{print $2}' | awk '{printf "%.4f", $1}'
}

# ============================================================
echo ""
echo "========================================"
echo " Building"
echo "========================================"
make -s clean && make -s || { echo "BUILD FAILED"; exit 1; }

# ============================================================
echo ""
echo "========================================"
echo " TEST 1: Single process"
echo " All four schedulers: no queuing, no waiting"
echo "========================================"
OUT=$(./scheduler inputs/test_single.txt)

for ALGO in "FCFS" "SJF" "Round Robin" "Priority"; do
    S=$(get_section "$ALGO" "$OUT")
    check "[$ALGO] completion=5" "$(get_field P1 Completion "$S")" "5"
    check "[$ALGO] waiting=0"    "$(get_field P1 Waiting    "$S")" "0"
    check "[$ALGO] response=0"   "$(get_field P1 Response   "$S")" "0"
done

# ============================================================
echo ""
echo "========================================"
echo " TEST 2: Idle CPU gap"
echo " P1 finishes at t=2; P2 arrives at t=5"
echo " Expect: CPU idle t=2..4 → util = 62.50%"
echo "========================================"
OUT=$(./scheduler inputs/test_idle.txt)

for ALGO in "FCFS" "Round Robin" "Priority"; do
    S=$(get_section "$ALGO" "$OUT")
    check "[$ALGO] P2 waiting=0"     "$(get_field P2 Waiting    "$S")" "0"
    check "[$ALGO] P2 response=0"    "$(get_field P2 Response   "$S")" "0"
    check "[$ALGO] P2 completion=8"  "$(get_field P2 Completion "$S")" "8"
    check "[$ALGO] CPU util=62.50"   "$(get_metric 'CPU Utilization' "$S")" "62.50"
done

# ============================================================
echo ""
echo "========================================"
echo " TEST 3: RR — burst < quantum"
echo " P1(burst=1), P2(burst=3), quantum=2"
echo " P1 finishes inside its first quantum"
echo "========================================"
OUT=$(./scheduler inputs/test_burst_lt_quantum.txt)
S=$(get_section "Round Robin" "$OUT")

check "[RR] P1 completion=1"  "$(get_field P1 Completion "$S")" "1"
check "[RR] P1 waiting=0"     "$(get_field P1 Waiting    "$S")" "0"
check "[RR] P1 response=0"    "$(get_field P1 Response   "$S")" "0"
check "[RR] P2 start=1"       "$(get_field P2 Start      "$S")" "1"
check "[RR] P2 completion=4"  "$(get_field P2 Completion "$S")" "4"
check "[RR] P2 waiting=1"     "$(get_field P2 Waiting    "$S")" "1"

# ============================================================
echo ""
echo "========================================"
echo " TEST 4: RR — burst exactly equals quantum"
echo " P1,P2,P3 each burst=2, quantum=2"
echo " No process needs re-enqueuing"
echo "========================================"
OUT=$(./scheduler inputs/test_burst_eq_quantum.txt)
S=$(get_section "Round Robin" "$OUT")

check "[RR] P1 completion=2"  "$(get_field P1 Completion "$S")" "2"
check "[RR] P2 completion=4"  "$(get_field P2 Completion "$S")" "4"
check "[RR] P3 completion=6"  "$(get_field P3 Completion "$S")" "6"
check "[RR] P1 waiting=0"     "$(get_field P1 Waiting    "$S")" "0"
check "[RR] P2 waiting=2"     "$(get_field P2 Waiting    "$S")" "2"
check "[RR] P3 waiting=4"     "$(get_field P3 Waiting    "$S")" "4"

# ============================================================
echo ""
echo "========================================"
echo " TEST 5: Priority — lower number wins"
echo " P1(prio=3) runs first (only one arrived);"
echo " then P2(prio=1) beats P3(prio=2)"
echo "========================================"
OUT=$(./scheduler inputs/test_priority_order.txt)
S=$(get_section "Priority" "$OUT")

check "[Priority] P1 start=0"       "$(get_field P1 Start      "$S")" "0"
check "[Priority] P1 completion=4"  "$(get_field P1 Completion "$S")" "4"
check "[Priority] P2 start=4"       "$(get_field P2 Start      "$S")" "4"
check "[Priority] P2 completion=7"  "$(get_field P2 Completion "$S")" "7"
check "[Priority] P3 start=7"       "$(get_field P3 Start      "$S")" "7"
check "[Priority] P3 completion=9"  "$(get_field P3 Completion "$S")" "9"

# ============================================================
echo ""
echo "========================================"
echo " TEST 6: Priority — tie-breaking (same priority)"
echo " P2 and P3 both prio=2; P2 arrived first → runs first"
echo "========================================"
OUT=$(./scheduler inputs/test_priority_tie.txt)
S=$(get_section "Priority" "$OUT")

p2start=$(get_field P2 Start "$S")
p3start=$(get_field P3 Start "$S")
earlier=$(awk -v a="$p2start" -v b="$p3start" 'BEGIN{print (a<b)?"yes":"no"}')
check "[Priority-tie] P2 starts before P3"  "$earlier" "yes"
check "[Priority-tie] P1 completion=3"      "$(get_field P1 Completion "$S")" "3"
check "[Priority-tie] P2 completion=7"      "$(get_field P2 Completion "$S")" "7"
check "[Priority-tie] P3 completion=9"      "$(get_field P3 Completion "$S")" "9"

# ============================================================
echo ""
echo "========================================"
echo " TEST 7: Metrics — workload1 known values"
echo " (hand-verified by full trace)"
echo "========================================"
OUT=$(./scheduler inputs/workload1.txt)

S=$(get_section "Round Robin" "$OUT")
check "[Metrics/RR] avg waiting=9.25"       "$(get_metric "Average Waiting"          "$S")" "9.25"
check "[Metrics/RR] avg turnaround=14.75"   "$(get_metric "Average Turnaround"       "$S")" "14.75"
check "[Metrics/RR] avg response=2.50"      "$(get_metric "Average Response"         "$S")" "2.50"
check "[Metrics/RR] CPU util=100.00"        "$(get_metric 'CPU Utilization'  "$S")" "100.00"
check "[Metrics/RR] throughput=0.1818"      "$(get_throughput                "$S")" "0.1818"

S=$(get_section "Priority" "$OUT")
check "[Metrics/Prio] avg waiting=5.25"     "$(get_metric "Average Waiting"          "$S")" "5.25"
check "[Metrics/Prio] avg turnaround=10.75" "$(get_metric "Average Turnaround"       "$S")" "10.75"
check "[Metrics/Prio] avg response=5.25"    "$(get_metric "Average Response"         "$S")" "5.25"

# ============================================================
echo ""
echo "========================================"
printf " Results: ${GREEN}%d passed${NC}  ${RED}%d failed${NC}\n" $PASS $FAIL
echo "========================================"
[ $FAIL -eq 0 ] && exit 0 || exit 1
