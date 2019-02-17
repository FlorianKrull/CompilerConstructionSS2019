# Submission Guideline

- `XX` is to be replaced with the number of your team with leading zero (e.g. `02`).
- `Y` is to be replaced with the corresponding milestone number.
- One submission *per team*.

## Example Input Submission

Assuming your example input is named `mandelbrot`, zip the corresponding files like so:

    mandelbrot.zip
    └── mandelbrot/
        ├── mandelbrot.mc
        ├── mandelbrot.stdin.txt
        └── mandelbrot.stdout.txt

Submit the zip archive via mail using the following line as subject (or link below).
List your team members in the mail body.

    703602 - Example Input

📧 [send email](mailto:alexander.hirsch@uibk.ac.at?subject=703602%20-%20Example%20Input)

## Milestone Submission

1. `cd` into your repository.
2. Commit all pending changes.
3. Checkout the revision you want to submit.
4. Ensure everything builds.
    - Warnings are okay
    - Tests may fail
    - Memory may be leaked
    - Known issues should be present
5. Run the following command:

        $ git archive --prefix=team_XX_milestone_Y/ --format=zip HEAD > team_XX_milestone_Y.zip

6. Verify that the resulting archive contains everything you want to submit and nothing more.
7. Submit the zip archive via mail using the following line as subject (or link below).

        703602 - Team XX Milestone Y

    📧 [send email](mailto:alexander.hirsch@uibk.ac.at?subject=703602%20-%20Team%20XX%20Milestone%20Y)

## Final Submission

1. `cd` into your repository.
2. Commit all pending changes.
3. Checkout the revision you want to submit.
4. Ensure everything works.
    - Everything builds
    - No (unjustified) warnings
    - All unit tests succeed
    - All integration tests succeed
    - No memory is leaked
    - Known issues must be present
5. Run the following command:

        $ git archive --prefix=team_XX_final/ --format=zip HEAD > team_XX_final.zip

6. Verify that the resulting archive contains everything you want to submit and nothing more.
7. Submit the zip archive via mail using the following line as subject (or link below).

        703602 - Team XX Final

    📧 [send email](mailto:alexander.hirsch@uibk.ac.at?subject=703602%20-%20Team%20XX%20Final)
