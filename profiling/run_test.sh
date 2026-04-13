#!/usr/bin/env bash
# =============================================================================
# VLSPQC test launcher
# Usage: bash run_test.sh [CATEGORY [FAMILY [TEST_NUMBER]]]
#   CATEGORY   : common | profiled
#   FAMILY     : algorithm family name (e.g. ML-KEM, FALCON, HQC …)
#   TEST_NUMBER: 1-based index of variant within the chosen family
# =============================================================================
set -euo pipefail

# Script is located in profiling/, ROOT points to repository root
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# ---------------------------------------------------------------------------
# Family classification helpers
# ---------------------------------------------------------------------------
get_family_common() {
  local p="$1"
  case "$p" in
    masked/ASCON/*)                           echo "ASCON"   ;;
    original/ASCON/*)                         echo "ASCON"   ;;
    original/DS/CROSS/*)                      echo "CROSS"   ;;
    other/CROSS-test/*)                       echo "CROSS"   ;;
    original/DS/FALCON/*)                     echo "FALCON"  ;;
    original/DS/LESS/*)                       echo "LESS"    ;;
    original/DS/ML-DSA/*)                     echo "ML-DSA"  ;;
    original/DS/SLH-DSA|original/DS/SLH-DSA/*) echo "SLH-DSA" ;;
    original/ML-KEM/*)                        echo "ML-KEM"  ;;
    other/share-resources/*)                  echo "tests"   ;;
    *)                          echo "other"   ;;
  esac
}

get_family_profiled() {
  local bn
  bn="$(basename "$1")"
  case "$bn" in
    ML-DSA*|ml-dsa*)       echo "ML-DSA"  ;;
    SPHINCS*|sphincs*|SLH*) echo "SLH-DSA" ;;
    HQC*|hqc*)             echo "HQC"     ;;
    ml-kem*|ML-KEM*)       echo "ML-KEM"  ;;
    falcon*|FALCON*)       echo "FALCON"  ;;
    *)                     echo "other"   ;;
  esac
}

# ---------------------------------------------------------------------------
# Discover all test directories for a given category
# ---------------------------------------------------------------------------
discover_paths() {
  local mode="$1"
  find "$ROOT" -type f -name Makefile -printf "%h\n" \
    | sed "s#^$ROOT/##" | sort -u \
    | while IFS= read -r d; do
        [[ -f "$ROOT/$d/main.c" ]] || continue
        if [[ "$mode" == "common" ]]; then
          [[ "$d" == profiling/* ]] && continue
          [[ "$d" == PROFILING/* ]] && continue
        else
          if [[ "$d" != profiling/* && "$d" != PROFILING/* ]]; then
            continue
          fi
        fi
        echo "$d"
      done
}

# ---------------------------------------------------------------------------
# Pretty variant label: "basename  (parent/dir)"
# ---------------------------------------------------------------------------
variant_label() {
  local p="$1"
  local bn parent
  bn="$(basename "$p")"
  parent="$(dirname "$p")"
  if [[ "$parent" == "." ]]; then
    echo "$bn"
  else
    echo "$bn  [$parent]"
  fi
}

# ---------------------------------------------------------------------------
# Print a divider
# ---------------------------------------------------------------------------
hr() { echo "─────────────────────────────────────────────────"; }

# ============================================================================
# Entry point
# ============================================================================
CATEGORY="${1:-}"
FAMILY="${2:-}"
TEST_IDX="${3:-}"

# ── Step 1: category ──────────────────────────────────────────────────────
if [[ -z "$CATEGORY" ]]; then
  hr
  echo "  VLSPQC Test Launcher"
  hr
  echo "  Select test set:"
  echo ""
  echo "    1)  common    — original, unmodified implementations"
  echo "    2)  profiled  — instrumented versions in PROFILING/"
  echo ""
  read -r -p "  Enter number: " choice
  case "$choice" in
    1) CATEGORY="common"   ;;
    2) CATEGORY="profiled" ;;
    *) echo "Invalid selection."; exit 1 ;;
  esac
fi

case "$CATEGORY" in
  common|profiled) ;;
  *) echo "Invalid CATEGORY: $CATEGORY  (use: common | profiled)"; exit 1 ;;
esac

# ── Step 2: discover paths ────────────────────────────────────────────────
mapfile -t all_paths < <(discover_paths "$CATEGORY")
if (( ${#all_paths[@]} == 0 )); then
  echo "No tests found for category: $CATEGORY"
  exit 1
fi

# ── Step 3: build unique sorted family list ───────────────────────────────
mapfile -t families < <(
  for p in "${all_paths[@]}"; do
    if [[ "$CATEGORY" == "common" ]]; then
      get_family_common "$p"
    else
      get_family_profiled "$p"
    fi
  done | sort -u
)

# ── Step 4: family selection ──────────────────────────────────────────────
if [[ -z "$FAMILY" ]]; then
  echo ""
  hr
  echo "  [$CATEGORY]  Select algorithm family:"
  hr
  for i in "${!families[@]}"; do
    printf "    %2d)  %s\n" $((i+1)) "${families[$i]}"
  done
  echo ""
  read -r -p "  Enter number: " fchoice
  if ! [[ "$fchoice" =~ ^[0-9]+$ ]] || (( fchoice < 1 || fchoice > ${#families[@]} )); then
    echo "Invalid selection."
    exit 1
  fi
  FAMILY="${families[$((fchoice-1))]}"
fi

# ── Step 5: filter variants for selected family ───────────────────────────
mapfile -t variants < <(
  for p in "${all_paths[@]}"; do
    local_fam=""
    if [[ "$CATEGORY" == "common" ]]; then
      local_fam="$(get_family_common "$p")"
    else
      local_fam="$(get_family_profiled "$p")"
    fi
    [[ "$local_fam" == "$FAMILY" ]] && echo "$p"
  done
)

if (( ${#variants[@]} == 0 )); then
  echo "No tests found for family: $FAMILY"
  exit 1
fi

# ── Step 6: variant selection ─────────────────────────────────────────────
if [[ -z "$TEST_IDX" ]]; then
  if (( ${#variants[@]} == 1 )); then
    TEST_IDX=1
    echo ""
    echo "  Only one variant available — selecting: $(variant_label "${variants[0]}")"
  else
    echo ""
    hr
    echo "  [$CATEGORY › $FAMILY]  Select variant:"
    hr
    for i in "${!variants[@]}"; do
      printf "    %2d)  %s\n" $((i+1)) "$(variant_label "${variants[$i]}")"
    done
    echo ""
    read -r -p "  Enter number: " TEST_IDX
  fi
fi

if ! [[ "$TEST_IDX" =~ ^[0-9]+$ ]] || (( TEST_IDX < 1 || TEST_IDX > ${#variants[@]} )); then
  echo "Invalid test number."
  exit 1
fi

# ── Step 7: build & run ───────────────────────────────────────────────────
relpath="${variants[$((TEST_IDX-1))]}"
label="$(basename "$relpath")"
absdir="$ROOT/$relpath"

echo ""
hr
echo "  Running: $CATEGORY › $FAMILY › $label"
echo "  Path:    $relpath"
hr
echo ""

if [[ ! -d "$absdir" ]]; then
  echo "Directory not found: $absdir"
  exit 1
fi

(
  cd "$absdir"
  make clean
  make
  echo ""
  bin="./$(basename "$PWD")"
  if [[ -x "$bin" ]]; then
    exec "$bin"
  fi
  fallback=$(find . -maxdepth 1 -type f -perm -111 | head -n 1)
  if [[ -n "$fallback" ]]; then
    exec "$fallback"
  fi
  echo "No executable produced in $absdir"
  exit 1
)
