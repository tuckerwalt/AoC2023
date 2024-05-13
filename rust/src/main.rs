use clap::Parser;
use std::fs;
use std::time::Instant;

pub mod day01;

#[derive(Parser,Default,)]
struct Args {
    // Day to run, 01 to 25
    day: String,

    // Problem input file to run against, defaults to the test input for the day
    #[clap(default_value_t=("testinput.txt").to_string())]
    input: String,

    // -t to print timings of each part
    #[clap(short, long, default_value_t=false)]
    time: bool,
}

/// Parse arguments, open/read the given input file and pass the contents to 
/// the solving functions.
fn main() {
    let args = Args::parse();
    let (day, input) = (args.day, args.input);
    let inputfile: String = format!("src/day{day}/{input}");
    let should_time = args.time;
    let file_contents = fs::read_to_string(&inputfile).expect(format!("{inputfile} does not exist").as_str());
    
    match day.as_str() {
        "01" => {
            run_function(day01::part1, &file_contents, should_time);
            run_function(day01::part2, &file_contents, should_time);
            run_function(day01::part2_replace, &file_contents, should_time);
            run_function(day01::part2_idiomatic, &file_contents, should_time);
        }
        day => {
            println!("Day {day} not implemented");
        }
    }
}

/// Runs the specified argument, timing the function if requested.
fn run_function(func: fn(&String), input: &String, should_time: bool) {
    match should_time {
        true => {
            let start = Instant::now();
            func(input);
            let duration = start.elapsed();
            println!("function took {:?}", duration);
        },
        false => {
            func(input);
        },
    }
}