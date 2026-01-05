#!/usr/bin/env python3
#
# Copyright (C) 2005-2025 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

"""
ITT API Test Runner

This script runs ITT API tests using the reference collector.
It sets up the environment, runs each test, and reports results.

Usage:
    python run_tests.py [options]

Options:
    --build-dir DIR     Path to build directory (default: ./build)
    --refcol-lib PATH   Path to reference collector library
    --log-dir DIR       Directory for log files (default: /tmp/itt_test_logs)
    --verbose           Enable verbose output
    --filter PATTERN    Run only tests matching pattern
    --help              Show this help message

Examples:
    python run_tests.py --build-dir ./build
    python run_tests.py --filter "domain" --verbose
    python run_tests.py --log-dir /var/log/itt_tests
"""

import argparse
import glob
import os
import platform
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional


# =============================================================================
# ANSI Color Support
# =============================================================================

class Colors:
    """ANSI color codes for terminal output."""
    
    # Check if colors should be enabled
    _enabled = sys.stdout.isatty() and os.environ.get('NO_COLOR') is None
    
    RED = '\033[1;31m' if _enabled else ''
    GREEN = '\033[1;32m' if _enabled else ''
    YELLOW = '\033[1;33m' if _enabled else ''
    BLUE = '\033[1;34m' if _enabled else ''
    CYAN = '\033[1;36m' if _enabled else ''
    RESET = '\033[0m' if _enabled else ''
    
    @classmethod
    def disable(cls):
        """Disable all colors."""
        cls.RED = cls.GREEN = cls.YELLOW = cls.BLUE = cls.CYAN = cls.RESET = ''


# =============================================================================
# Data Classes
# =============================================================================

@dataclass
class TestResult:
    """Result of a single test execution."""
    name: str
    passed: bool
    exit_code: int
    duration: float
    output: str = ""


@dataclass
class TestSummary:
    """Summary of all test results."""
    total: int = 0
    passed: int = 0
    failed: int = 0
    results: List[TestResult] = field(default_factory=list)
    
    @property
    def failed_tests(self) -> List[TestResult]:
        return [r for r in self.results if not r.passed]


# =============================================================================
# Logging Functions
# =============================================================================

def print_header():
    """Print the test suite header."""
    print()
    print(f"{Colors.CYAN}═══════════════════════════════════════════════════════════════{Colors.RESET}")
    print(f"{Colors.CYAN}                ITT API Integration Test Suite                 {Colors.RESET}")
    print(f"{Colors.CYAN}═══════════════════════════════════════════════════════════════{Colors.RESET}")
    print()


def print_section(title: str):
    """Print a section header."""
    print()
    print(f"{Colors.BLUE}───────────────────────────────────────────────────────────────{Colors.RESET}")
    print(f"{Colors.BLUE}  {title}{Colors.RESET}")
    print(f"{Colors.BLUE}───────────────────────────────────────────────────────────────{Colors.RESET}")


def log_info(message: str):
    """Print an info message."""
    print(f"{Colors.BLUE}[INFO]{Colors.RESET} {message}")


def log_success(message: str):
    """Print a success message."""
    print(f"{Colors.GREEN}[PASS]{Colors.RESET} {message}")


def log_error(message: str):
    """Print an error message."""
    print(f"{Colors.RED}[FAIL]{Colors.RESET} {message}")


def log_warn(message: str):
    """Print a warning message."""
    print(f"{Colors.YELLOW}[WARN]{Colors.RESET} {message}")


# =============================================================================
# Platform Utilities
# =============================================================================

def get_platform_info() -> dict:
    """Get platform-specific information."""
    system = platform.system().lower()
    
    if system == 'windows':
        return {
            'system': 'windows',
            'lib_extension': '.dll',
            'exe_extension': '.exe',
            'env_var': 'INTEL_LIBITTNOTIFY64',
            'default_log_dir': os.path.join(os.environ.get('TEMP', 'C:\\Temp'), 'itt_test_logs'),
            'path_sep': ';',
        }
    else:  # Linux, macOS, etc.
        return {
            'system': system,
            'lib_extension': '.so' if system == 'linux' else '.dylib',
            'exe_extension': '',
            'env_var': 'INTEL_LIBITTNOTIFY64',
            'default_log_dir': '/tmp/itt_test_logs',
            'path_sep': ':',
        }


def find_test_executables(test_dir: Path, pattern: Optional[str] = None) -> List[Path]:
    """Find all test executables in the given directory."""
    platform_info = get_platform_info()
    exe_ext = platform_info['exe_extension']
    
    executables = []
    
    # Look for test_* executables
    for item in test_dir.iterdir():
        if not item.is_file():
            continue
            
        name = item.name
        
        # Check if it matches test_* pattern
        if exe_ext:
            if not name.startswith('test_') or not name.endswith(exe_ext):
                continue
        else:
            if not name.startswith('test_'):
                continue
            # On Unix, check if it's executable
            if not os.access(item, os.X_OK):
                continue
        
        # Apply filter if specified
        if pattern and not re.search(pattern, name, re.IGNORECASE):
            continue
            
        executables.append(item)
    
    return sorted(executables)


def find_refcol_library(build_dir: Path) -> Optional[Path]:
    """Find the reference collector library."""
    platform_info = get_platform_info()
    lib_ext = platform_info['lib_extension']
    
    # Common locations to search
    search_paths = [
        build_dir / 'lib' / f'libittnotify_refcol{lib_ext}',
        build_dir / f'libittnotify_refcol{lib_ext}',
        build_dir / 'Release' / f'ittnotify_refcol{lib_ext}',
        build_dir / 'Debug' / f'ittnotify_refcol{lib_ext}',
    ]
    
    for path in search_paths:
        if path.exists():
            return path
    
    return None


# =============================================================================
# Test Runner
# =============================================================================

class TestRunner:
    """Runs ITT API tests with the reference collector."""
    
    def __init__(
        self,
        build_dir: Path,
        refcol_lib: Optional[Path] = None,
        log_dir: Optional[Path] = None,
        verbose: bool = False,
        filter_pattern: Optional[str] = None,
    ):
        self.build_dir = Path(build_dir).resolve()
        self.refcol_lib = Path(refcol_lib).resolve() if refcol_lib else None
        self.log_dir = Path(log_dir).resolve() if log_dir else None
        self.verbose = verbose
        self.filter_pattern = filter_pattern
        self.platform_info = get_platform_info()
        self.summary = TestSummary()
        
    def setup_environment(self) -> bool:
        """Set up the test environment."""
        print_section("Setting up test environment")
        
        # Validate build directory
        if not self.build_dir.exists():
            log_error(f"Build directory not found: {self.build_dir}")
            return False
        
        # Find reference collector library
        if not self.refcol_lib:
            self.refcol_lib = find_refcol_library(self.build_dir)
        
        if not self.refcol_lib or not self.refcol_lib.exists():
            log_error(f"Reference collector library not found")
            return False
        
        log_info(f"Reference collector: {self.refcol_lib}")
        
        # Set up log directory
        if not self.log_dir:
            self.log_dir = Path(self.platform_info['default_log_dir'])
        
        self.log_dir.mkdir(parents=True, exist_ok=True)
        log_info(f"Log directory: {self.log_dir}")
        
        # Clean up old log files
        self._cleanup_logs()
        log_info("Cleaned up old log files")
        
        return True
    
    def _cleanup_logs(self):
        """Remove old log files from the log directory."""
        if not self.log_dir.exists():
            return
            
        for log_file in self.log_dir.glob('libittnotify_refcol_*.log'):
            try:
                log_file.unlink()
            except OSError:
                pass
    
    def _get_test_env(self) -> dict:
        """Get environment variables for running tests."""
        env = os.environ.copy()
        env['INTEL_LIBITTNOTIFY64'] = str(self.refcol_lib)
        env['INTEL_LIBITTNOTIFY_LOG_DIR'] = str(self.log_dir)
        return env
    
    def run_single_test(self, test_path: Path) -> TestResult:
        """Run a single test executable."""
        test_name = test_path.name
        
        # Clean logs before test
        self._cleanup_logs()
        
        # Build command
        cmd = [str(test_path)]
        if self.verbose:
            cmd.append('--verbose')
        
        # Run the test
        start_time = time.time()
        
        try:
            if self.verbose:
                # Show output in real-time
                print()
                print(f"{Colors.CYAN}>>> Running: {test_name}{Colors.RESET}")
                result = subprocess.run(
                    cmd,
                    env=self._get_test_env(),
                    timeout=300,  # 5 minute timeout
                )
                output = ""
            else:
                # Capture output
                result = subprocess.run(
                    cmd,
                    env=self._get_test_env(),
                    capture_output=True,
                    text=True,
                    timeout=300,
                )
                output = result.stdout + result.stderr
            
            exit_code = result.returncode
            
        except subprocess.TimeoutExpired:
            exit_code = -1
            output = "Test timed out after 300 seconds"
        except Exception as e:
            exit_code = -1
            output = str(e)
        
        duration = time.time() - start_time
        passed = exit_code == 0
        
        return TestResult(
            name=test_name,
            passed=passed,
            exit_code=exit_code,
            duration=duration,
            output=output,
        )
    
    def run_all_tests(self) -> bool:
        """Run all tests."""
        print_section("Running tests")
        
        # Find test directory
        test_dir = self.build_dir / 'bin'
        if not test_dir.exists():
            # Try alternative locations
            if (self.build_dir / 'Release').exists():
                test_dir = self.build_dir / 'Release'
            elif (self.build_dir / 'Debug').exists():
                test_dir = self.build_dir / 'Debug'
            else:
                log_error(f"Test directory not found: {test_dir}")
                return False
        
        # Find test executables
        executables = find_test_executables(test_dir, self.filter_pattern)
        
        if not executables:
            log_warn(f"No test executables found in {test_dir}")
            return True
        
        log_info(f"Found {len(executables)} test executable(s)")
        print()
        
        # Run each test
        for test_path in executables:
            result = self.run_single_test(test_path)
            self.summary.results.append(result)
            self.summary.total += 1
            
            if result.passed:
                self.summary.passed += 1
                if not self.verbose:
                    log_success(test_path.name)
            else:
                self.summary.failed += 1
                log_error(f"{test_path.name} (exit code: {result.exit_code})")
                if result.output and not self.verbose:
                    # Show first few lines of output on failure
                    lines = result.output.strip().split('\n')[:10]
                    for line in lines:
                        print(f"    {line}")
        
        return True
    
    def print_summary(self):
        """Print the test summary."""
        print_section("Test Summary")
        
        print(f"  Total:   {Colors.CYAN}{self.summary.total}{Colors.RESET}")
        print(f"  Passed:  {Colors.GREEN}{self.summary.passed}{Colors.RESET}")
        print(f"  Failed:  {Colors.RED}{self.summary.failed}{Colors.RESET}")
        
        if self.summary.failed_tests:
            print()
            print(f"{Colors.RED}Failed tests:{Colors.RESET}")
            for result in self.summary.failed_tests:
                print(f"  • {result.name}")
        
        print()
        if self.summary.failed == 0:
            print(f"{Colors.GREEN}✓ All tests passed!{Colors.RESET}")
        else:
            print(f"{Colors.RED}✗ Some tests failed!{Colors.RESET}")
        print()
    
    def run(self) -> int:
        """Run the complete test suite. Returns exit code."""
        print_header()
        
        if not self.setup_environment():
            return 1
        
        if not self.run_all_tests():
            return 1
        
        self.print_summary()
        
        return 0 if self.summary.failed == 0 else 1


# =============================================================================
# Main Entry Point
# =============================================================================

def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description='ITT API Test Runner',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    %(prog)s --build-dir ./build
    %(prog)s --filter "domain" --verbose
    %(prog)s --log-dir /var/log/itt_tests
        """,
    )
    
    parser.add_argument(
        '--build-dir',
        type=Path,
        default=Path('./build'),
        help='Path to build directory (default: ./build)',
    )
    
    parser.add_argument(
        '--refcol-lib',
        type=Path,
        default=None,
        help='Path to reference collector library (auto-detected if not specified)',
    )
    
    parser.add_argument(
        '--log-dir',
        type=Path,
        default=None,
        help='Directory for log files (default: platform-specific temp directory)',
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose output',
    )
    
    parser.add_argument(
        '--filter', '-f',
        type=str,
        default=None,
        dest='filter_pattern',
        help='Run only tests matching pattern (case-insensitive regex)',
    )
    
    parser.add_argument(
        '--no-color',
        action='store_true',
        help='Disable colored output',
    )
    
    return parser.parse_args()


def main() -> int:
    """Main entry point."""
    args = parse_args()
    
    if args.no_color:
        Colors.disable()
    
    runner = TestRunner(
        build_dir=args.build_dir,
        refcol_lib=args.refcol_lib,
        log_dir=args.log_dir,
        verbose=args.verbose,
        filter_pattern=args.filter_pattern,
    )
    
    return runner.run()


if __name__ == '__main__':
    sys.exit(main())
