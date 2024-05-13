/**
 * Get the sum of the calibration values of each line in the input
 * ex: a1b2c3d4e5f
 * The calibration value is the first and last digit in the string (15)
 * 
 * For part two, a spelled-out digit also counts, meaning
 * 7pqrstsixteen has a calibration value of 76
 */

/// Scans each line from the front and back, finding the first digit
/// encountered and adding to the total calibration
pub fn part1(input: &String) {
    let mut calibration: u32 = 0;
    for line in input.lines() {
        for char in line.chars() {
            if char.is_digit(10) {
                calibration += 10 * char.to_digit(10).unwrap();
                break;
            }
        }
        for char in line.chars().rev() {
            if char.is_digit(10) {
                calibration += char.to_digit(10).unwrap();
                break;
            }
        }
    }

    println!("Part 1: Calibration is {calibration}");
}

/// Scans lines similar to part one, but searches for spelled out numbers along the way.
/// Short-circuits when it finds the first/last number
pub fn part2(input: &String) {
    let numstrings = vec!["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"];

    let calibration: u32 = input.lines().map(|line| {
        let mut calibration = 0;
        'line: for (ind, char) in line.char_indices() {
            if char.is_digit(10) {
                calibration += 10 * char.to_digit(10).unwrap();
                break;
            }
            /// Look for each spelled num
            for (nind, numstr) in (&numstrings).into_iter().enumerate() {
                if line[ind..].starts_with(numstr) {
                    calibration += 10 * nind as u32;
                    break 'line;
                }
            }
        }
        'linerev: for (ind, char) in line.char_indices().rev() {
            if char.is_digit(10) {
                calibration += char.to_digit(10).unwrap();
                break;
            }
            for (nind, numstr) in (&numstrings).into_iter().enumerate() {
                if line[ind..].starts_with(numstr) {
                    calibration += nind as u32;
                    break 'linerev;
                }
            }
        }
        calibration
    }).sum();

    println!("Part 2: Calibration is {calibration}");
}

/// Replaces each spelled-out digit with a similar word containing the actual digit,
/// so we can just use the basic part one logic to find the first and last number.
pub fn part2_replace(input: &String) {
    let mut calibration: u32 = 0;
    let numstrings = vec!["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"];
    let fixedstrings = vec!["ze0o", "o1e", "t2o", "th3ee", "f4ur", "f5ve", "s6x", "se7en", "ei8ht", "n9ne"];

    for line in input.lines() {
        let mut newline = String::from(line);
        for (ind, numberstring) in (&numstrings).into_iter().enumerate() {
            newline = newline.replace(numberstring, fixedstrings[ind]);
        }
        for char in newline.chars() {
            if char.is_digit(10) {
                calibration += 10 * char.to_digit(10).unwrap();
                break;
            }
        }
        for char in newline.chars().rev() {
            if char.is_digit(10) {
                calibration += char.to_digit(10).unwrap();
                break;
            }
        }
    }

    println!("Part 2 (replace): Calibration is {calibration}");
}

/// Compact but slightly slower implementation, replacing spelled-out digits before
/// getting the first and last numbers. Reduces the string into a vector of just the 
/// numbers to easily extract the needed numbers.
pub fn part2_idiomatic(input: &String) {
    let calibration: u32 = input.lines().map(|line| {
        let digits: Vec<u32> = line.
            replace("one", "o1e").
            replace("two", "t2o").
            replace("three", "th3ee").
            replace("four", "fo4r").
            replace("five", "f5ve").
            replace("six", "s6x").
            replace("seven", "se7en").
            replace("eight", "ei8ht").
            replace("nine", "n9ne").
            chars().filter_map(|char| char.to_digit(10)).collect();
        10 * digits.first().unwrap() + digits.last().unwrap()
    }).sum();
    println!("Part 2 (idiomatic): Calibration is {calibration}");
}