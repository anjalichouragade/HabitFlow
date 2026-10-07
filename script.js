// ==========================================
// HABITFLOW - DAILY HABIT TRACKER
// ==========================================

let habits = JSON.parse(localStorage.getItem("habitflowHabits")) || [];

let editingId = null;


// ==========================================
// SAVE DATA
// ==========================================

function saveData() {
    localStorage.setItem("habitflowHabits", JSON.stringify(habits));
}


// ==========================================
// DATE FUNCTIONS
// ==========================================

function getTodayKey() {
    const today = new Date();

    const year = today.getFullYear();
    const month = String(today.getMonth() + 1).padStart(2, "0");
    const day = String(today.getDate()).padStart(2, "0");

    return `${year}-${month}-${day}`;
}


function formatDate(date) {
    return date.toLocaleDateString("en-IN", {
        day: "numeric",
        month: "long",
        year: "numeric"
    });
}


function displayTodayDate() {
    document.getElementById("todayDate").textContent =
        formatDate(new Date());
}


// ==========================================
// HABIT ICONS
// ==========================================

function getHabitIcon(category) {

    const cat = category.toLowerCase();

    if (cat.includes("study"))
        return "📚";

    if (cat.includes("health"))
        return "💪";

    if (cat.includes("exercise"))
        return "🏃";

    if (cat.includes("fitness"))
        return "🏃";

    if (cat.includes("read"))
        return "📖";

    if (cat.includes("sleep"))
        return "😴";

    if (cat.includes("work"))
        return "💻";

    if (cat.includes("water"))
        return "💧";

    if (cat.includes("meditat"))
        return "🧘";

    return "🌱";
}


// ==========================================
// CURRENT STREAK
// ==========================================

function getCurrentStreak(habit) {

    if (!habit.completedDates ||
        habit.completedDates.length === 0) {
        return 0;
    }

    let streak = 0;
    let date = new Date();

    while (true) {

        const key = getDateKey(date);

        if (habit.completedDates.includes(key)) {
            streak++;
            date.setDate(date.getDate() - 1);
        } else {
            break;
        }

        if (streak > 3650)
            break;
    }

    return streak;
}


// ==========================================
// LONGEST STREAK
// ==========================================

function getLongestStreak(habit) {

    if (!habit.completedDates ||
        habit.completedDates.length === 0) {
        return 0;
    }

    const dates = [...habit.completedDates].sort();

    let longest = 1;
    let current = 1;

    for (let i = 1; i < dates.length; i++) {

        const previous = new Date(dates[i - 1]);
        const currentDate = new Date(dates[i]);

        const difference =
            (currentDate - previous) / (1000 * 60 * 60 * 24);

        if (difference === 1) {
            current++;
        } else {
            current = 1;
        }

        if (current > longest)
            longest = current;
    }

    return longest;
}


// ==========================================
// DATE KEY
// ==========================================

function getDateKey(date) {

    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, "0");
    const day = String(date.getDate()).padStart(2, "0");

    return `${year}-${month}-${day}`;
}


// ==========================================
// TOGGLE HABIT
// ==========================================

function toggleHabit(id) {

    const habit = habits.find(h => h.id === id);

    if (!habit)
        return;

    if (!habit.completedDates)
        habit.completedDates = [];

    const today = getTodayKey();

    const index = habit.completedDates.indexOf(today);

    if (index === -1) {
        habit.completedDates.push(today);
    } else {
        habit.completedDates.splice(index, 1);
    }

    saveData();
    render();
}


// ==========================================
// RENDER HABITS
// ==========================================

function renderHabits() {

    const list = document.getElementById("habitList");
    const empty = document.getElementById("emptyState");

    list.innerHTML = "";

    if (habits.length === 0) {
        empty.style.display = "block";
        return;
    }

    empty.style.display = "none";

    const today = getTodayKey();

    habits.forEach(habit => {

        const completed =
            habit.completedDates &&
            habit.completedDates.includes(today);

        const card = document.createElement("div");

        card.className = "habit-card";

        card.innerHTML = `

            <button
                class="checkbox ${completed ? "checked" : ""}"
                onclick="toggleHabit(${habit.id})"
            >
                ${completed ? "✓" : ""}
            </button>

            <div class="habit-info">

                <div class="habit-name">
                    ${getHabitIcon(habit.category)}
                    ${escapeHTML(habit.name)}
                </div>

                <div class="habit-category">
                    ${escapeHTML(habit.category)}
                </div>

            </div>

            <div class="habit-streak">
                🔥 ${getCurrentStreak(habit)} day${getCurrentStreak(habit) === 1 ? "" : "s"}
            </div>

            <div class="habit-actions">

                <button
                    class="icon-btn"
                    onclick="editHabit(${habit.id})"
                    title="Edit"
                >
                    ✎
                </button>

                <button
                    class="icon-btn"
                    onclick="deleteHabit(${habit.id})"
                    title="Delete"
                >
                    🗑
                </button>

            </div>
        `;

        list.appendChild(card);
    });
}


// ==========================================
// UPDATE STATISTICS
// ==========================================

function updateStats() {

    const total = habits.length;

    const today = getTodayKey();

    let completed = 0;

    habits.forEach(habit => {

        if (
            habit.completedDates &&
            habit.completedDates.includes(today)
        ) {
            completed++;
        }

    });

    let percentage = 0;

    if (total > 0) {
        percentage = Math.round((completed / total) * 100);
    }

    document.getElementById("totalHabits").textContent = total;

    document.getElementById("completionRate").textContent =
        percentage + "%";


    // Overall streak
    let overallStreak = 0;

    habits.forEach(habit => {

        const streak = getCurrentStreak(habit);

        if (streak > overallStreak) {
            overallStreak = streak;
        }

    });

    document.getElementById("overallStreak").textContent =
        overallStreak;
}


// ==========================================
// WEEKLY PROGRESS
// ==========================================

function renderWeeklyProgress() {

    const container =
        document.getElementById("weeklyProgress");

    container.innerHTML = "";

    const today = new Date();

    const dayNames =
        ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];


    for (let i = 6; i >= 0; i--) {

        const date = new Date(today);

        date.setDate(today.getDate() - i);

        const key = getDateKey(date);

        let completed = false;

        if (habits.length > 0) {

            completed = habits.some(habit =>

                habit.completedDates &&
                habit.completedDates.includes(key)

            );

        }

        const day = document.createElement("div");

        day.className = "day-box";

        day.innerHTML = `

            <div class="day-name">
                ${dayNames[date.getDay()]}
            </div>

            <div class="day-circle ${completed ? "complete" : ""}">
                ${completed ? "✓" : "○"}
            </div>

        `;

        container.appendChild(day);
    }
}


// ==========================================
// OPEN MODAL
// ==========================================

function openModal() {

    editingId = null;

    document.getElementById("modalTitle").textContent =
        "Add Habit";

    document.getElementById("habitName").value = "";

    document.getElementById("habitCategory").value = "";

    document.getElementById("habitModal").classList.add("show");

    document.getElementById("habitName").focus();
}


// ==========================================
// CLOSE MODAL
// ==========================================

function closeModal() {

    document.getElementById("habitModal")
        .classList.remove("show");

    editingId = null;
}


// ==========================================
// SAVE HABIT
// ==========================================

function saveHabit() {

    const name =
        document.getElementById("habitName")
            .value.trim();

    const category =
        document.getElementById("habitCategory")
            .value.trim();


    if (name === "") {
        alert("Please enter a habit name.");
        return;
    }


    if (category === "") {
        alert("Please enter a category.");
        return;
    }


    // EDIT
    if (editingId !== null) {

        const habit =
            habits.find(h => h.id === editingId);

        if (habit) {

            habit.name = name;
            habit.category = category;

        }

    }

    // ADD
    else {

        const newHabit = {

            id: Date.now(),

            name: name,

            category: category,

            completedDates: []

        };

        habits.push(newHabit);
    }


    saveData();

    closeModal();

    render();
}


// ==========================================
// EDIT HABIT
// ==========================================

function editHabit(id) {

    const habit =
        habits.find(h => h.id === id);

    if (!habit)
        return;

    editingId = id;

    document.getElementById("modalTitle").textContent =
        "Edit Habit";

    document.getElementById("habitName").value =
        habit.name;

    document.getElementById("habitCategory").value =
        habit.category;

    document.getElementById("habitModal")
        .classList.add("show");
}


// ==========================================
// DELETE HABIT
// ==========================================

function deleteHabit(id) {

    const habit =
        habits.find(h => h.id === id);

    if (!habit)
        return;


    const confirmDelete =
        confirm(
            `Delete "${habit.name}"?`
        );

    if (!confirmDelete)
        return;


    habits =
        habits.filter(h => h.id !== id);

    saveData();

    render();
}


// ==========================================
// ESCAPE HTML
// ==========================================

function escapeHTML(text) {

    const div = document.createElement("div");

    div.textContent = text;

    return div.innerHTML;
}


// ==========================================
// MAIN RENDER
// ==========================================

function render() {

    renderHabits();

    updateStats();

    renderWeeklyProgress();
}


// ==========================================
// MODAL OUTSIDE CLICK
// ==========================================

document.getElementById("habitModal")
    .addEventListener("click", function(event) {

        if (event.target === this) {
            closeModal();
        }

    });


// ==========================================
// START APPLICATION
// ==========================================

displayTodayDate();

render();